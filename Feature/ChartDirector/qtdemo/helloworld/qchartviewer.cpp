#ifndef QT_NO_DEPRECATED_WARNINGS
#define QT_NO_DEPRECATED_WARNINGS
#endif
#include "qchartviewer.h"
#include <time.h>
#include <QApplication>
#include <QMouseEvent>
#include <QToolTip>
#include <QCursor>
#include <QBitmap>
#include <QFile>


/////////////////////////////////////////////////////////////////////////////
//
// QChartImage
//
/////////////////////////////////////////////////////////////////////////////

QChartImage::QChartImage(QWidget *parent) :
    QLabel(parent)
{
    // High dpi support
    m_dpi = 0;
    m_toImageScaleX = 1;
    m_toImageScaleY = 1;

    setMouseTracking(true);
}

QChartImage::~QChartImage()
{
}

void QChartImage::displayChart(BaseChart *c)
{
    // Render chart into QPixmap
    QPixmap image;
    if (0 != c)
    {
        int dpi = getDpi();
        if (dpi > 0)
        {
            char buffer[1024];
            sprintf(buffer, "+dpi=%d", dpi);
            c->setOutputOptions(buffer);
        }
        MemBlock m = c->makeChart(Chart::QTIMG);
        image.loadFromData((uchar *)m.data, (uint)m.len);
        double pixelRatio = (dpi > 0) ? devicePixelRatioF() : 1;
        image.setDevicePixelRatio(pixelRatio);
        setFixedSize((int)(image.width() / pixelRatio + 0.5), (int)(image.height() / pixelRatio + 0.5));
    }
    setPixmap(image);

    // High dpi support
    m_toImageScaleX = ((width() <= 0) || !c) ? 1 : (c->getDrawArea()->getWidth() / (double)width());
    m_toImageScaleY = ((height() <= 0) || !c) ? 1 : (c->getDrawArea()->getHeight() / (double)height());
}

void QChartImage::setDpi(int dpi)
{
    m_dpi = dpi;
}

int QChartImage::getDpi()
{
    int dpi = m_dpi;
    if (dpi < 0)
        return dpi;
    if (dpi == 0)
        dpi = (int)(96 * devicePixelRatioF() + 0.5);
    return (dpi < 24) ? 24 : ((dpi > 384) ? 384 : dpi);
}

double QChartImage::toImageX(int x)
{
    return x * m_toImageScaleX;
}
double QChartImage::toImageY(int y)
{
    return y * m_toImageScaleY;
}

int QChartImage::toDisplayX(double x)
{
    double ret = x / m_toImageScaleX;
    return (int)((ret >= 0) ? ret + 0.5 : (ret - 0.5));
}
int QChartImage::toDisplayY(double y)
{
    double ret = y / m_toImageScaleY;
    return (int)((ret >= 0) ? ret + 0.5 : (ret - 0.5));
}


/////////////////////////////////////////////////////////////////////////////
//
// Mouse cursors for zooming and scrolling support
//
/////////////////////////////////////////////////////////////////////////////

static class QASE_CursorManager
{
private:
    QCursor *hZoomInCursor;
    QCursor *hZoomOutCursor;
    QCursor *hNoZoomCursor;

    QCursor &getZoomCursor(QCursor **cache, double scale, int flags)
    {
        if (0 == *cache)
        {
            DrawArea d;
            d.setSize(32, 32, Chart::Transparent);
            d.circle(15, 15, 7, 7, 0x000000, 0xffffff);
            if (flags & 0x1)
                d.hline(12, 18, 15, 0x000000);
            if (flags & 0x2)
                d.vline(12, 18, 15, 0x000000);
            int x[] = { 21, 28, 26, 19 };
            int y[] = { 19, 26, 28, 21 };
            d.polygon(IntArray(x, 4), IntArray(y, 4), 0x000000, 0x000000);
            char buffer[1024];
            sprintf(buffer, "dpi=%d", (int)(96 * scale + 0.5));
            d.setOutputOptions(buffer);
            MemBlock m = d.outPNG();
            QImage img = QImage::fromData((const uchar*)m.data, m.len);
            img.setDevicePixelRatio(scale);
            *cache = new QCursor(QPixmap::fromImage(img), 15, 15);
        }
        return **cache;
    }

public:
    QASE_CursorManager() :
        hZoomInCursor(0), hZoomOutCursor(0), hNoZoomCursor(0)
    {
    }
    ~QASE_CursorManager()
    {
        delete hZoomInCursor;
        delete hZoomOutCursor;
        delete hNoZoomCursor;
    }
    QCursor &getZoomInCursor(double scale)
    {
        return getZoomCursor(&hZoomInCursor, scale, 3);
    }
    QCursor &getZoomOutCursor(double scale)
    {
        return getZoomCursor(&hZoomOutCursor, scale, 1);
    }
    QCursor &getNoZoomCursor(double scale)
    {
        return getZoomCursor(&hNoZoomCursor, scale, 0);
    }
} cursorManager;


/////////////////////////////////////////////////////////////////////////////
//
// QChartViewer
//
/////////////////////////////////////////////////////////////////////////////

//
// Constants used in m_delayChartUpdate
//
enum { NO_DELAY, NEED_DELAY, NEED_UPDATE };

QChartViewer::QChartViewer(QWidget *parent) :
    QChartImage(parent)
{
    // current chart and hot spot tester
    m_currentChart = 0;
    m_hotSpotTester = 0;
    m_imageMapChart = 0;

    // initialize chart configuration
    m_selectBoxLineColor = QColor(0, 0, 0);
    m_selectBoxLineWidth = 2;
    m_mouseUsage = Chart::MouseUsageDefault;
    m_zoomDirection = Chart::DirectionHorizontal;
    m_zoomInRatio = 2;
    m_zoomOutRatio = 0.5;
    m_mouseWheelZoomRatio = 1;
    m_scrollDirection = Chart::DirectionHorizontal;
    m_minDragAmount = 5;
    m_updateInterval = 20;

    // current state of the mouse
    m_isOnPlotArea = false;
    m_isPlotAreaMouseDown = false;
    m_isDragScrolling = false;
    m_currentHotSpot = -1;
    m_isClickable = false;
    m_isMouseTracking = false;
    m_isInMouseMove = false;

    // chart update rate support
    m_needUpdateChart = false;
    m_needUpdateImageMap = false;
    m_holdTimerActive = false;
    m_isInViewPortChanged = false;
    m_delayUpdateChart = NO_DELAY;
    m_delayedChart = 0;
    m_lastMouseMove = 0;
    m_delayedMouseEvent = 0;
    m_delayImageMapUpdate = false;

    // track cursor support
    m_autoHideMsg = "";
    m_currentMouseX = -0x3fffffff;
    m_currentMouseY = -0x3fffffff;
    m_isInMouseMovePlotArea = false;

    // selection rectangle
    m_LeftLine = 0;
    m_RightLine = 0;
    m_TopLine = 0;
    m_BottomLine = 0;

    // tooltip
    m_enhancedToolTip = 0;
    m_CDMLToolTipPrefix = "<*block,bgColor=F0F0F0,edgeColor=808080,margin=5,roundedCorners=3*><*font,color=222222*>";
}

QChartViewer::~QChartViewer()
{
    delete m_hotSpotTester;
    delete m_delayedMouseEvent;
    delete m_delayedChart;
    delete m_imageMapChart;
}

namespace {
    static class QtResourceLoader {
    public:
        QtResourceLoader() { Chart::setResourceLoader(loader); }
        static bool loader(const char *id, char *(allocator)(int), char **data, int *len) {
            QFile f(QString(":/") + id);
            return f.open(QIODevice::ReadOnly) && (0 < (*len = f.size())) &&
                (0 != (*data = allocator(*len))) && f.read(*data, *len);
        }
    } QtResourceLoaderRegistration;
}

/////////////////////////////////////////////////////////////////////////////
// QChartViewer message handlers

//
// MouseMove event handler
//
void QChartViewer::mouseMoveEvent(QMouseEvent *event)
{
    // Enable mouse tracking to detect mouse leave events
    m_isMouseTracking = true;

    emit mouseMove(event);

    // On Windows, mouse events can by-pass the event queue. If there are too many mouse events,
    // the event queue may not get processed, preventing other controls from updating. If two mouse
    // events are less than 10ms apart, there is a risk of too many mouse events. So we repost the
    // mouse event as a timer event that is queued up normally, allowing the queue to get processed.
    unsigned int timeBetweenMouseMove = (((unsigned int)clock()) - m_lastMouseMove)
                                        * 1000 / CLOCKS_PER_SEC ;
    if ((m_delayedMouseEvent && (timeBetweenMouseMove < 250)) || (timeBetweenMouseMove < 10))
    {
        if (!m_delayedMouseEvent)
            m_delayedMouseEventTimerId = startTimer(1);
        else
            delete m_delayedMouseEvent;

        m_delayedMouseEvent = new QMouseEvent(event->type(), event->pos(), event->globalPos(),
            event->button(), event->buttons(), event->modifiers());
    }
    else
        commitMouseMove(event);

    onSetCursor();
}

//
// The method that actually performs MouseMove event processing
//
void QChartViewer::commitMouseMove(QMouseEvent *event)
{
    // Remember the mouse coordinates for later use
    m_currentMouseX = event->x();
    m_currentMouseY = event->y();

    // The chart can be updated more than once during mouse move. For example, it can update due to
    // drag to scroll, and also due to drawing track cursor. So we delay updating the display until
    // all all events has occured.
    m_delayUpdateChart = NEED_DELAY;
    m_isInMouseMove = true;

    // Check if mouse is dragging on the plot area
    m_isOnPlotArea = m_isPlotAreaMouseDown || inPlotArea(toImageX(event->x()), toImageY(event->y()));
    if (m_isPlotAreaMouseDown)
        onPlotAreaMouseDrag(event);

    // Emit mouseMoveChart
    emit mouseMoveChart(event);

    if (inExtendedPlotArea(toImageX(event->x()), toImageY(event->y())))
    {
        // Mouse is in extended plot area, emit mouseMovePlotArea
        m_isInMouseMovePlotArea = true;
        emit mouseMovePlotArea(event);
    }
    else if (m_isInMouseMovePlotArea)
    {
        // Mouse was in extended plot area, but is not in it now, so emit mouseLeavePlotArea
        m_isInMouseMovePlotArea = false;
        emit mouseLeavePlotArea(event);
        applyAutoHide("mouseleaveplotarea");
    }

    //
    // Show hot spot tool tips if necessary
    // (Due to QT bug, tooltip cannot be put in delayedMouseEvent, otherwise sometimes tooltip
    // position will be incorrect.)
    //

    if (m_delayImageMapUpdate)
    {
        m_delayImageMapUpdate = false;
        if (!m_isPlotAreaMouseDown)
            updateViewPort(false, true);
    }

    // Use the ChartDirector ImageMapHandler to determine if the mouse is over a hot spot
    int hotSpotNo = 0;
    if (0 != m_hotSpotTester)
        hotSpotNo = m_hotSpotTester->getHotSpot(toImageX(event->x()), toImageY(event->y()), m_imageMapChart);

    const char* isDynamic = 0;
    if (hotSpotNo != 0)
        isDynamic = m_hotSpotTester->getValue("dynamic");

    // Hide	classical tool tip if mouse button is pressed.
    if (event->buttons() != Qt::NoButton)
        QToolTip::hideText();

    // If the mouse is in the same hot spot since the last mouse move event, there is no need
    // to update the tool tip.
    if ((hotSpotNo != m_currentHotSpot) || ((0 != isDynamic) && (0 != *isDynamic)))
    {
        // Hot spot has changed - update tool tip text
        m_currentHotSpot = hotSpotNo;

        if (hotSpotNo == 0)
        {
            // Mouse is not actually on hanlder hot spot - use default tool tip text and reset
            // the clickable flag.
            m_isClickable = false;
            showToolTip(m_defaultToolTip);
        }
        else
        {
            // Mouse is on a hot spot. In this implementation, we consider the hot spot as
            // clickable if its href ("path") parameter is not empty.
            const char *path = m_hotSpotTester->getValue("path");
            m_isClickable = ((0 != path) && (0 != *path) && ('~' != *path));
            showToolTip(QString::fromUtf8(m_hotSpotTester->getValue("title")));
        }
    }

    if (m_enhancedToolTip && m_enhancedToolTip->isVisible())
        moveEnhancedToolTip(mapFromGlobal(QCursor::pos()));

    // Cancel the delayed mouse event if any
    if (m_delayedMouseEvent)
    {
        killTimer(m_delayedMouseEventTimerId);
        delete m_delayedMouseEvent;
        m_delayedMouseEvent = 0;
    }

    // Can update chart now
    commitUpdateChart();
    m_isInMouseMove = false;

    m_lastMouseMove = (unsigned int)clock();
}

void QChartViewer::showToolTip(const QString& text)
{
    if ((text.length() <= 0) || ((text[0] == '<') && (text[1] == '*')))
    {
        showEnhancedToolTip(text);
        QToolTip::showText(QCursor::pos(), "");
    }
    else
    {
        showEnhancedToolTip(0);

        if (QApplication::mouseButtons() != Qt::NoButton)
            QToolTip::showText(QCursor::pos(), "");
        else
            QToolTip::showText(QCursor::pos(), text);
    }
}

void QChartViewer::showEnhancedToolTip(const QString& text)
{
    if (0 == m_enhancedToolTip)
    {
        m_enhancedToolTip = new QChartImage(this);
        m_enhancedToolTip->setStyleSheet("background-color:rgba(255,255,255,0)");
    }

    if (0 >= text.length())
    {
        m_enhancedToolTip->hide();
        m_enhancedToolTip->displayChart(0);
        return;
    }

    QString buffer = text;
    buffer.replace("<*cdml*>", m_CDMLToolTipPrefix);

    PieChart c(1, 1, Chart::Transparent);
    c.makeChart()->renderCDML(buffer.toUtf8().append('\0'));
    m_enhancedToolTip->setDpi(getDpi());
    m_enhancedToolTip->displayChart(&c);
    m_enhancedToolTip->show();

    moveEnhancedToolTip(mapFromGlobal(QCursor::pos()));
}


void QChartViewer::moveEnhancedToolTip(const QPoint &p)
{
    int x = p.x();
    int y = p.y() + 20;

    if (x + m_enhancedToolTip->width() > width())
    {
        x = width() - m_enhancedToolTip->width();
        if (x < 0)
            x = 0;
    }
    if (y + m_enhancedToolTip->height() > height())
    {
        y = p.y() - m_enhancedToolTip->height() - 2;
        if (y < 0)
            y = 0;
    }

    m_enhancedToolTip->move(x, y);
}

//
// Delayed MouseMove event handler
//
void QChartViewer::onDelayedMouseMove()
{
    if (m_delayedMouseEvent)
        commitMouseMove(m_delayedMouseEvent);
}

//
// MouseLeave handler
//
void QChartViewer::leaveEvent(QEvent *event)
{
    // Process delayed mouse move, if any
    onDelayedMouseMove();

    // Mouse tracking is no longer active
    m_isMouseTracking = false;

    if (m_isInMouseMovePlotArea)
    {
        // Mouse was in extended plot area, but is not in it now, so emit mouseLeavePlotArea
        m_isInMouseMovePlotArea = false;
        emit mouseLeavePlotArea(event);
        applyAutoHide("mouseleaveplotarea");
    }

    // emit mouseLeaveChart
    emit mouseLeaveChart(event);
    applyAutoHide("mouseleavechart");

    // hide tooltip
    showEnhancedToolTip(0);
}

//
// MouseWheel handler
//
void QChartViewer::wheelEvent(QWheelEvent *event)
{
    bool hasReceivers = receivers(SIGNAL(mouseWheel(QWheelEvent *))) > 0;
    if (hasReceivers)
    {
        // Process delayed mouse move, if any
        onDelayedMouseMove();

        // emit mouseWheel event
        emit mouseWheel(event);
    }

    // Process the mouse wheel only if the mouse is over the plot area
    bool hasMouseWheelZoom = isMouseOnPlotArea() &&
        onMouseWheelZoom(getPlotAreaMouseX(), getPlotAreaMouseY(), event->angleDelta().y());

    if (!(hasReceivers || hasMouseWheelZoom))
        event->ignore();
}

bool QChartViewer::onMouseWheelZoom(int x, int y, int zDelta)
{
    // Zoom ratio = 1 means no zooming
    if (m_mouseWheelZoomRatio == 1)
        return false;

    // X and Y zoom ratios
    double rx = 1;
    double ry = 1;
    if (getZoomDirection() != Chart::DirectionVertical)
        rx = (zDelta > 0) ? m_mouseWheelZoomRatio : 1 / m_mouseWheelZoomRatio;
    if (getZoomDirection() != Chart::DirectionHorizontal)
        ry = (zDelta > 0) ? m_mouseWheelZoomRatio : 1 / m_mouseWheelZoomRatio;

    // Do the zooming
    if (zoomAround(x, y, rx, ry))
        updateViewPort(true, true);

    return true;
}

//
// Change the mouse cursor.
//
void QChartViewer::onSetCursor()
{
    if (this->m_isDragScrolling)
    {
        switch (m_scrollDirection)
        {
        case Chart::DirectionHorizontal:
            //setCursor(getNoMoveHorizCursor());
            setCursor(Qt::SizeHorCursor);
            break;
        case Chart::DirectionVertical:
            //setCursor(getNoMoveVertCursor());
            setCursor(Qt::SizeVerCursor);
            break;
        default :
            //setCursor(getNoMove2DCursor());
            setCursor(Qt::SizeAllCursor);
            break;
        }

        return;
    }

    if (m_isOnPlotArea)
    {
        switch (m_mouseUsage)
        {
        case Chart::MouseUsageZoomIn:
            if (canZoomIn(m_zoomDirection))
                setCursor(cursorManager.getZoomInCursor(devicePixelRatio()));
            else
                setCursor(cursorManager.getNoZoomCursor(devicePixelRatio()));
            return;
        case Chart::MouseUsageZoomOut:
            if (canZoomOut(m_zoomDirection))
                setCursor(cursorManager.getZoomOutCursor(devicePixelRatio()));
            else
                setCursor(cursorManager.getNoZoomCursor(devicePixelRatio()));
            return;
        }
    }

    if (m_isClickable)
        setCursor(Qt::PointingHandCursor);
    else
        unsetCursor();
}

//
// Mouse button down event.
//
void QChartViewer::mousePressEvent(QMouseEvent *event)
{
    onDelayedMouseMove();

    if ((event->button() == Qt::LeftButton) && inPlotArea(toImageX(event->x()), toImageY(event->y())) &&
        (m_mouseUsage != Chart::MouseUsageDefault))
    {
        // Mouse usage is for drag to zoom/scroll. Capture the mouse to prepare for dragging and
        // save the mouse down position to draw the selection rectangle.
        m_isPlotAreaMouseDown = true;
        m_plotAreaMouseDownXPos = event->x();
        m_plotAreaMouseDownYPos = event->y();
        startDrag();
    }
}

//
// Mouse button up event.
//
void QChartViewer::mouseReleaseEvent(QMouseEvent *event)
{
    onDelayedMouseMove();

    if ((event->button() == Qt::LeftButton) && m_isPlotAreaMouseDown)
    {
        // Release the mouse capture.
        m_isPlotAreaMouseDown = false;
        setRectVisible(false);
        bool hasUpdate = false;

        switch (m_mouseUsage)
        {
        case Chart::MouseUsageZoomIn :
            if (canZoomIn(m_zoomDirection))
            {
                if (isDrag(m_zoomDirection, event))
                {
                    int minX, minY, spanX, spanY;
                    getDragZoomRect(event->x(), event->y(), &minX, &minY, &spanX, &spanY);

                    // Zoom to the drag selection rectangle.
                    hasUpdate = zoomTo(m_zoomDirection,
                        toImageX(minX), toImageY(minY), toImageX(minX + spanX), toImageY(minY + spanY));
                }
                else
                    // User just click on a point. Zoom-in around the mouse cursor position.
                    hasUpdate = zoomAt(m_zoomDirection, toImageX(event->x()), toImageY(event->y()), m_zoomInRatio);
            }
            break;
        case Chart::MouseUsageZoomOut:
            // Zoom out around the mouse cursor position.
            if (canZoomOut(m_zoomDirection))
                hasUpdate = zoomAt(m_zoomDirection, toImageX(event->x()), toImageY(event->y()), m_zoomOutRatio);
            break;
        default :
            if (m_isDragScrolling)
                // Drag to scroll. We can update the image map now as scrolling has finished.
                updateViewPort(false, true);
            else
                // Is not zooming or scrolling, so is just a normal click event.
                emit clicked(event);
            break;
        }

        m_isDragScrolling = false;
        if (hasUpdate)
            // View port has changed - update it.
            updateViewPort(true, true);
    }
    else
        emit clicked(event);

    onSetCursor();
}

//
// Chart hold timer.
//
void QChartViewer::timerEvent(QTimerEvent *event)
{
    if (m_delayedMouseEvent && (event->timerId() == m_delayedMouseEventTimerId))
    {
        // Is a delayed mouse move event
        onDelayedMouseMove();
    }
    else if (m_holdTimerActive && (event->timerId() == m_holdTimerId))
    {
        killTimer(m_holdTimerId);
        m_holdTimerActive = false;

        // If has pending chart update request, handles them now.
        if (m_needUpdateChart || m_needUpdateImageMap)
            updateViewPort(m_needUpdateChart, m_needUpdateImageMap);
    }
}

/////////////////////////////////////////////////////////////////////////////
// QChartViewer properties

//
// Set the chart to the control
//
void QChartViewer::setChart(BaseChart *c)
{
    if (m_currentChart != c)
        setImageMap(0);

    m_currentChart = c;
    delete m_imageMapChart;
    m_imageMapChart = c ? new BaseChart(c) : 0;

    if (0 != c)
    {
        commitPendingSyncAxis(c);
        if (m_delayUpdateChart != NO_DELAY)
            c->makeChart();
    }

    updateDisplay();
}

//
// Get back the same BaseChart pointer provided by the previous setChart call.
//
BaseChart *QChartViewer::getChart()
{
    return m_currentChart;
}

//
// Set image map used by the chart
//
void QChartViewer::setImageMap(const char *imageMap)
{
    //delete the existing ImageMapHandler
    if (0 != m_hotSpotTester)
        delete m_hotSpotTester;
    m_currentHotSpot = -1;
    m_isClickable = false;

    //create a new ImageMapHandler to represent the image map
    if ((0 == imageMap) || (0 == *imageMap))
        m_hotSpotTester = 0;
    else
        m_hotSpotTester = new ImageMapHandler(imageMap, this);
}

//
// Get the image map handler for the chart
//
ImageMapHandler *QChartViewer::getImageMapHandler()
{
    return m_hotSpotTester;
}

//
// Set the default tool tip to use
//
void QChartViewer::setDefaultToolTip(const QString& text)
{
    m_defaultToolTip = text;
}

//
// Set the CDML tool tip prefix
//
void QChartViewer::setCDMLToolTipPrefix(const QString& prefix)
{
    m_CDMLToolTipPrefix = prefix;
}

//
// Set the border width of the selection box
//
void QChartViewer::setSelectionBorderWidth(int width)
{
    m_selectBoxLineWidth = width;
}

//
// Get the border with of the selection box.
//
int QChartViewer::getSelectionBorderWidth()
{
    return m_selectBoxLineWidth;
}

//
// Set the border color of the selection box
//
void QChartViewer::setSelectionBorderColor(QColor c)
{
    m_selectBoxLineColor = c;
    if (m_TopLine != 0)
    {
        QPalette p(c, c);
        m_TopLine->setPalette(p);
        m_LeftLine->setPalette(p);
        m_BottomLine->setPalette(p);
        m_RightLine->setPalette(p);
    }
}

//
// Get the border color of the selection box.
//
QColor QChartViewer::getSelectionBorderColor()
{
    return m_selectBoxLineColor;
}

//
// Set the mouse usage mode
//
void QChartViewer::setMouseUsage(int mouseUsage)
{
    m_mouseUsage = mouseUsage;
}

//
// Get the mouse usage mode
//
int QChartViewer::getMouseUsage()
{
    return m_mouseUsage;
}

//
// Set the zoom direction
//
void QChartViewer::setZoomDirection(int direction)
{
    m_zoomDirection = direction;
    setKeepAspectRatio(m_zoomDirection == Chart::KeepAspectRatio);
}

//
// Get the zoom direction
//
int QChartViewer::getZoomDirection()
{
    return m_zoomDirection;
}

//
// Set the scroll direction
//
void QChartViewer::setScrollDirection(int direction)
{
    m_scrollDirection = direction;
}

//
// Get the scroll direction
//
int QChartViewer::getScrollDirection()
{
    return m_scrollDirection;
}

//
// Set the zoom-in ratio for mouse click zoom-in
//
void QChartViewer::setZoomInRatio(double ratio)
{
    m_zoomInRatio = ratio;
}

//
// Get the zoom-in ratio for mouse click zoom-in
//
double QChartViewer::getZoomInRatio()
{
    return m_zoomInRatio;
}

//
// Set the zoom-out ratio
//
void QChartViewer::setZoomOutRatio(double ratio)
{
    m_zoomOutRatio = ratio;
}

//
// Get the zoom-out ratio
//
double QChartViewer::getZoomOutRatio()
{
    return m_zoomOutRatio;
}

//
// Set the mouse wheel zoom ratio 
//
void QChartViewer::setMouseWheelZoomRatio(double ratio)
{
	m_mouseWheelZoomRatio = ratio;
}

//
// Get the mouse wheel zoom ratio 
//
double QChartViewer::getMouseWheelZoomRatio()
{
	return m_mouseWheelZoomRatio;	
}

//
// Set the minimum mouse drag before the dragging is considered as real. This is to avoid small
// mouse vibrations triggering a mouse drag.
//
void QChartViewer::setMinimumDrag(int offset)
{
    m_minDragAmount = offset;
}

//
// Get the minimum mouse drag before the dragging is considered as real.
//
int QChartViewer::getMinimumDrag()
{
    return m_minDragAmount;
}

//
// Set the minimum interval between ViewPortChanged events. This is to avoid the chart being
// updated too frequently. (Default is 20ms between chart updates.) Multiple update events
// arrived during the interval will be merged into one chart update and executed at the end
// of the interval.
//
void QChartViewer::setUpdateInterval(int interval)
{
    m_updateInterval = interval;
}

//
// Get the minimum interval between ViewPortChanged events.
//
int QChartViewer::getUpdateInterval()
{
    return m_updateInterval;
}

//
// Check if there is a pending chart update request.
//
bool QChartViewer::needUpdateChart()
{
    return m_needUpdateChart;
}

//
// Check if there is a pending image map update request.
//
bool QChartViewer::needUpdateImageMap()
{
    return m_needUpdateImageMap;
}

//
// Get the current mouse x coordinate when used in a mouse move event handler
//
int QChartViewer::getChartMouseX()
{  
    if (m_currentMouseX < -0x10000000)
        return getPlotAreaLeft() + getPlotAreaWidth();
    else
        return (int)(toImageX(m_currentMouseX) + 0.5);
}

//
// Get the current mouse y coordinate when used in a mouse move event handler
//
int QChartViewer::getChartMouseY()
{
    if (m_currentMouseY < -0x10000000)
        return getPlotAreaTop() + getPlotAreaHeight();
    else
        return (int)(toImageY(m_currentMouseY) + 0.5);
}

//
// Get the current mouse x coordinate bounded to the plot area when used in a mouse event handler
//
int QChartViewer::getPlotAreaMouseX()
{
    int ret = getChartMouseX();
    if (ret < getPlotAreaLeft())
        ret = getPlotAreaLeft();
    if (ret > getPlotAreaLeft() + getPlotAreaWidth())
        ret = getPlotAreaLeft() + getPlotAreaWidth();
    return ret;
}

//
// Get the current mouse y coordinate bounded to the plot area when used in a mouse event handler
//
int QChartViewer::getPlotAreaMouseY()
{
    int ret = getChartMouseY();
    if (ret < getPlotAreaTop())
        ret = getPlotAreaTop();
    if (ret > getPlotAreaTop() + getPlotAreaHeight())
        ret = getPlotAreaTop() + getPlotAreaHeight();
    return ret;
}

//
// Check if mouse is on the extended plot area
//
bool QChartViewer::isMouseOnPlotArea()
{
    if (m_isMouseTracking)
        return inExtendedPlotArea(getChartMouseX(), getChartMouseY());
    else
        return false;
}

//
// Check if mouse is dragging to scroll or to select the zoom rectangle
//
bool QChartViewer::isMouseDragging()
{
    return m_isPlotAreaMouseDown;
}

//
// Check if is currently processing a mouse move event
//
bool QChartViewer::isInMouseMoveEvent()
{
    return m_isInMouseMove;
}

//
// Check if is currently processing a view port changed event
//
bool QChartViewer::isInViewPortChangedEvent()
{
    return m_isInViewPortChanged;
}

/////////////////////////////////////////////////////////////////////////////
// QChartViewer methods

//
// Update the display
//
void QChartViewer::updateDisplay()
{
    if (m_delayUpdateChart == NO_DELAY)
        commitUpdateChart();
    else
    {
        m_delayUpdateChart = NEED_UPDATE;
        delete m_delayedChart;
        m_delayedChart = (0 != m_currentChart) ? new BaseChart(m_currentChart) : 0;
    }
}

//
// Commit chart to display
//
void QChartViewer::commitUpdateChart()
{
    if (m_delayUpdateChart == NEED_DELAY)
    {
        // No actual update occur
        m_delayUpdateChart = NO_DELAY;
        return;
    }

    // Display the chart
    BaseChart *c = (m_delayUpdateChart == NEED_UPDATE) ? m_delayedChart : m_currentChart;
    displayChart(c);

    // Set the chart metrics and clear the image map
    setChartMetrics(c ? c->getChartMetrics() : 0);

    // Any delayed chart has been committed
    m_delayUpdateChart = NO_DELAY;
    delete m_delayedChart;
    m_delayedChart = 0;
}

//
// Set the message used to remove the dynamic layer
//
void QChartViewer::removeDynamicLayer(const char *msg)
{
    m_autoHideMsg = QString(msg ? msg : "").toLower();
    if (m_autoHideMsg == "now")
        applyAutoHide(msg);
}

//
// Attempt to hide the dynamic layer using the specified message
//
void QChartViewer::applyAutoHide(const char *msg)
{
    if (m_autoHideMsg == msg)
    {
        if (0 != m_currentChart)
            m_currentChart->removeDynamicLayer();
        m_autoHideMsg = "";
        updateDisplay();
    }
}

//
// Create the edges for the selection rectangle
//
void QChartViewer::initRect()
{
    m_LeftLine = new QLabel(this);
    m_LeftLine->setAutoFillBackground(true);
    m_RightLine = new QLabel(this);
    m_RightLine->setAutoFillBackground(true);
    m_TopLine = new QLabel(this);
    m_TopLine->setAutoFillBackground(true);
    m_BottomLine = new QLabel(this);
    m_BottomLine->setAutoFillBackground(true);

    setSelectionBorderColor(getSelectionBorderColor());
}

//
// Set selection rectangle position and size
//
void QChartViewer::drawRect(int x, int y, int width, int height)
{
    // Create the edges of the rectangle if not already created
    if (m_TopLine == 0)
        initRect();

    // width < 0 is interpreted as the rectangle extends to the left or x.
    // height <0 is interpreted as the rectangle extends to above y.
    if (width < 0)
        x -= (width = -width);
    if (height < 0)
        y -= (height = -height);

    // Put the edges along the sides of the rectangle
    m_TopLine->move(x, y);
    m_TopLine->setFixedSize(width, m_selectBoxLineWidth);
    m_LeftLine->move(x, y);
    m_LeftLine->setFixedSize(m_selectBoxLineWidth, height);
    m_BottomLine->move(x, y + height - m_selectBoxLineWidth + 1);
    m_BottomLine->setFixedSize(width, m_selectBoxLineWidth);
    m_RightLine->move(x + width - m_selectBoxLineWidth + 1, y);
    m_RightLine->setFixedSize(m_selectBoxLineWidth, height);
}

//
// Show/hide selection rectangle
//
void QChartViewer::setRectVisible(bool b)
{
    // Create the edges of the rectangle if not already created
    if (b && (m_TopLine == 0))
        initRect();

    // Show/hide the edges
    if (m_TopLine != 0)
    {
        m_TopLine->setVisible(b);
        m_LeftLine->setVisible(b);
        m_BottomLine->setVisible(b);
        m_RightLine->setVisible(b);
    }
}

//
// Determines if the mouse is dragging.
//
bool QChartViewer::isDrag(int direction, QMouseEvent *event)
{
    // We only consider the mouse is dragging it is has dragged more than m_minDragAmount. This is
    // to avoid small mouse vibrations triggering a mouse drag.
    int spanX = abs(event->x() - m_plotAreaMouseDownXPos);
    int spanY = abs(event->y() - m_plotAreaMouseDownYPos);
    return ((direction != Chart::DirectionVertical) && (spanX >= m_minDragAmount)) ||
        ((direction != Chart::DirectionHorizontal) && (spanY >= m_minDragAmount));
}

void QChartViewer::getDragZoomRect(int px, int py, int* x, int* y, int* w, int* h)
{
    *x = (px < m_plotAreaMouseDownXPos) ? px : m_plotAreaMouseDownXPos;
    *y = (py < m_plotAreaMouseDownYPos) ? py : m_plotAreaMouseDownYPos;
    *w = abs(px - m_plotAreaMouseDownXPos);
    *h = abs(py - m_plotAreaMouseDownYPos);

    if (getZoomXYRatio() > 0)
    {
        double imageXYRatio = getPlotAreaWidth() / (double)getPlotAreaHeight();

        double whDelta = toImageX(*w) - toImageY(*h) * imageXYRatio;
        if (whDelta < 0)
            *w = toDisplayX(toImageY(*h) * imageXYRatio);
        else if (whDelta > 0)
            *h = toDisplayY(toImageX(*w) / imageXYRatio);

        if (*x == px)
            *x = m_plotAreaMouseDownXPos - *w;
        if (*y == py)
            *y = m_plotAreaMouseDownYPos - *h;
    }
}

//
// Process mouse dragging over the plot area
//
void QChartViewer::onPlotAreaMouseDrag(QMouseEvent *event)
{
    switch (m_mouseUsage)
    {
        case Chart::MouseUsageZoomIn :
        {
            //
            // Mouse is used for zoom in. Draw the selection rectangle if necessary.
            //

            bool isDragZoom = canZoomIn(m_zoomDirection) && isDrag(m_zoomDirection, event);
            if (isDragZoom)
            {
                int minX, minY, spanX, spanY;
                getDragZoomRect(event->x(), event->y(), &minX, &minY, &spanX, &spanY);

                switch (m_zoomDirection)
                {
                case Chart::DirectionHorizontal:
                    drawRect(minX, toDisplayY(getPlotAreaTop()), spanX, toDisplayY(getPlotAreaHeight()));
                    break;
                case Chart::DirectionVertical:
                    drawRect(toDisplayX(getPlotAreaLeft()), minY, toDisplayX(getPlotAreaWidth()), spanY);
                    break;
                default:
                    drawRect(minX, minY, spanX, spanY);
                    break;
                }
            }
            setRectVisible(isDragZoom);
            break;
        }
        case Chart::MouseUsageScroll :
        {
            //
            // Mouse is used for drag scrolling. Scroll and update the view port.
            //

            if (m_isDragScrolling || isDrag(m_scrollDirection, event))
            {
                m_isDragScrolling = true;
                if (dragTo(m_scrollDirection,
                    toImageX(event->x() - m_plotAreaMouseDownXPos), toImageY(event->y() - m_plotAreaMouseDownYPos)))
                    updateViewPort(true, false);
            }
        }
    }
}

//
// Trigger the ViewPortChanged event
//
void QChartViewer::updateViewPort(bool needUpdateChart, bool needUpdateImageMap)
{
    // Already updating, no need to update again
    if (m_isInViewPortChanged)
        return;

    // Merge the current update requests with any pending requests.
    m_needUpdateChart = m_needUpdateChart || needUpdateChart;
    m_needUpdateImageMap = needUpdateImageMap;

    // Hold timer has not expired, so do not update chart immediately. Keep the requests pending.
    if (m_holdTimerActive)
    {
        if (!m_holdElapsedTimer.hasExpired(m_updateInterval))
            return;

        m_holdTimerActive = false;
        killTimer(m_holdTimerId);
    }

    // The chart can be updated more than once during mouse move. For example, it can update due to
    // drag to scroll, and also due to drawing track cursor. So we delay updating the display until
    // all all updates has occured.
    int hasDelayUpdate = (m_delayUpdateChart != NO_DELAY);
    if (!hasDelayUpdate)
        m_delayUpdateChart = NEED_DELAY;

    // Can trigger the ViewPortChanged event.
    validateViewPort();
    m_isInViewPortChanged = true;
    emit viewPortChanged();
    m_isInViewPortChanged = false;

    // Can update chart now
    if (!hasDelayUpdate)
        commitUpdateChart();

    // Clear any pending updates.
    m_needUpdateChart = false;
    m_needUpdateImageMap = false;

    // Set hold timer to prevent multiple chart updates within a short period.
    if (m_updateInterval > 0)
    {
        m_holdTimerActive = true;
        m_holdTimerId = startTimer(m_updateInterval);
        m_holdElapsedTimer.restart();
    }
}


/////////////////////////////////////////////////////////////////////////////
//
// QViewPortControl
//
/////////////////////////////////////////////////////////////////////////////

QViewPortControl::QViewPortControl(QWidget *parent) :
    QChartImage(parent)
{
    m_ChartViewer = 0;
    m_Chart = 0;

    m_mouseDownX = 0;
    m_mouseDownY = 0;
}

//
// Free	resources
//
QViewPortControl::~QViewPortControl()
{
}

//
// Set the CChartViewer to be associated with this control
//
void QViewPortControl::setViewer(QChartViewer *viewer)
{
    if (0 != m_ChartViewer)
        // disconnect from previous QChartViewer
        disconnect(m_ChartViewer, 0, this, 0);
    m_ChartViewer = viewer;
    setViewPortManager(viewer);
    if (0 != m_ChartViewer)
        // Connect to new QChartViewer
        connect(m_ChartViewer, SIGNAL(viewPortChanged()), SLOT(onViewPortChanged()));

    updateDisplay();
}

//
// Get back	the	same QChartViewer pointer provided	by the previous	setViewer call.
//
QChartViewer *QViewPortControl::getViewer()
{
    return m_ChartViewer;
}

//
// Set the chart to	be displayed in the	control
//
void QViewPortControl::setChart(BaseChart *c)
{
    m_Chart = c;   
    ViewPortControlBase::setChart(c);
    updateDisplay();
}

//
// Get back	the	same BaseChart pointer provided	by the previous	setChart call.
//
BaseChart *QViewPortControl::getChart()
{
    return m_Chart;
}

//
// Display the chart
//
void QViewPortControl::paintDisplay()
{   
    displayChart(getChart());
}

//
// Handle the ViewPortChanged event from the associated QChartViewer
//
void QViewPortControl::onViewPortChanged()
{
    updateDisplay();
}

//
// Mouse button down event.
//
void QViewPortControl::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    // Remember current mouse position
    m_mouseDownX = event->x();
    m_mouseDownY = event->y();

    // Get the QChartViewer zoom/scroll state to determine which type of mouse action is allowed
    syncState();

    // Handle the mouse down event
    handleMouseDown(toImageX(event->x()), toImageY(event->y()));

    // Update the chart viewer if the viewport has changed
    updateChartViewerIfNecessary();
}

//
// MouseMove event handler
//
void QViewPortControl::mouseMoveEvent(QMouseEvent *event)
{
    // Get the QChartViewer zoom/scroll state to determine which type of mouse action is allowed
    syncState();

    // Handle the mouse move event
    handleMouseMove(toImageX(event->x()), toImageY(event->y()), isDrag(event));

    // Update the chart viewer if the viewport has changed
    updateChartViewerIfNecessary();

    // Update the mouse cursor
    updateCursor(getCursor());

    // Update the display
    if (needUpdateDisplay())
        paintDisplay();
}

//
// Mouse button up event.
//
void QViewPortControl::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    // Get the QChartViewer zoom/scroll state to determine which type of mouse action is allowed
    syncState();

    // Handle the mouse up event
    handleMouseUp(toImageX(event->x()), toImageY(event->y()));

    // Update the chart viewer if the viewport has changed
    updateChartViewerIfNecessary();
}

//
// MouseWheel handler
//
void QViewPortControl::wheelEvent(QWheelEvent *event)
{
#if QT_VERSION >= 0x051400
    double wx = event->position().x();
    double wy = event->position().y();
#else
    double wx = event->x();
    double wy = event->y();
#endif

    // Process the mouse wheel only if the mouse is over the plot area
    if ((0 == m_ChartViewer) || (!isOnPlotArea(wx, wy)))
        event->ignore();
    else
    {
        // Ask the CChartViewer to zoom around the center of the chart
        int x = m_ChartViewer->getPlotAreaLeft() + m_ChartViewer->getPlotAreaWidth() / 2;
        int y = m_ChartViewer->getPlotAreaTop() + m_ChartViewer->getPlotAreaHeight() / 2;
        if (!m_ChartViewer->onMouseWheelZoom(x, y, event->angleDelta().y()))
            event->ignore();
    }
}

//
// Get the QChartViewer zoom/scroll state
//
void QViewPortControl::syncState()
{
    QChartViewer *viewer = getViewer();
    if (0 != viewer)
        setZoomScrollDirection(viewer->getZoomDirection(), viewer->getScrollDirection());
}

//
// Determines if the mouse is dragging.
//
bool QViewPortControl::isDrag(QMouseEvent *event)
{
    QChartViewer *viewer = getViewer();
    if (0 == viewer)
        return false;

    int minimumDrag = viewer->getMinimumDrag();
    int moveX = abs(m_mouseDownX - event->x());
    int moveY = abs(m_mouseDownY - event->y());
    return (moveX >= minimumDrag) || (moveY >= minimumDrag);
}

//
// Update the display
//
void QViewPortControl::updateDisplay()
{
    paintViewPort();
    paintDisplay();
}

//
// Update the QChartViewer if the viewport has changed
//
void QViewPortControl::updateChartViewerIfNecessary()
{
    QChartViewer *viewer = getViewer();
    if (0 == viewer)
        return;

    if (needUpdateChart() || needUpdateImageMap())
        viewer->updateViewPort(needUpdateChart(), needUpdateImageMap());
}

//
// Update the mouse cursor
//
void QViewPortControl::updateCursor(int position)
{
    switch (position)
    {
    case Chart::Left:
    case Chart::Right:
        setCursor(Qt::SizeHorCursor);
        return;
    case Chart::Top:
    case Chart::Bottom:
        setCursor(Qt::SizeVerCursor);
        return;
    case Chart::TopLeft:
    case Chart::BottomRight:
        setCursor(Qt::SizeFDiagCursor);
        return;
    case Chart::TopRight:
    case Chart::BottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        return;
    default:
        unsetCursor();
        return;
    }
}

