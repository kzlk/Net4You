QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    CustomGrip.h \
    Settings.h \
    Widget.h \
    app_function.h \
    mainwindow.h

FORMS += \
    main.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc \
    resources.qrc

DISTFILES += \
    icon.ico \
    icon.ico \
    images/icons/cil-3d.png \
    images/icons/cil-3d.png \
    images/icons/cil-4k.png \
    images/icons/cil-4k.png \
    images/icons/cil-account-logout.png \
    images/icons/cil-account-logout.png \
    images/icons/cil-action-redo.png \
    images/icons/cil-action-redo.png \
    images/icons/cil-action-undo.png \
    images/icons/cil-action-undo.png \
    images/icons/cil-airplane-mode-off.png \
    images/icons/cil-airplane-mode-off.png \
    images/icons/cil-airplane-mode.png \
    images/icons/cil-airplane-mode.png \
    images/icons/cil-alarm.png \
    images/icons/cil-alarm.png \
    images/icons/cil-align-center.png \
    images/icons/cil-align-center.png \
    images/icons/cil-align-left.png \
    images/icons/cil-align-left.png \
    images/icons/cil-align-right.png \
    images/icons/cil-align-right.png \
    images/icons/cil-arrow-bottom-2.png \
    images/icons/cil-arrow-bottom-2.png \
    images/icons/cil-arrow-bottom.png \
    images/icons/cil-arrow-bottom.png \
    images/icons/cil-arrow-circle-bottom.png \
    images/icons/cil-arrow-circle-bottom.png \
    images/icons/cil-arrow-circle-left.png \
    images/icons/cil-arrow-circle-left.png \
    images/icons/cil-arrow-circle-right.png \
    images/icons/cil-arrow-circle-right.png \
    images/icons/cil-arrow-circle-top.png \
    images/icons/cil-arrow-circle-top.png \
    images/icons/cil-arrow-left.png \
    images/icons/cil-arrow-left.png \
    images/icons/cil-arrow-right.png \
    images/icons/cil-arrow-right.png \
    images/icons/cil-arrow-top.png \
    images/icons/cil-arrow-top.png \
    images/icons/cil-at.png \
    images/icons/cil-at.png \
    images/icons/cil-av-timer.png \
    images/icons/cil-av-timer.png \
    images/icons/cil-ban.png \
    images/icons/cil-ban.png \
    images/icons/cil-battery-0.png \
    images/icons/cil-battery-0.png \
    images/icons/cil-battery-3.png \
    images/icons/cil-battery-3.png \
    images/icons/cil-battery-5.png \
    images/icons/cil-battery-5.png \
    images/icons/cil-battery-alert.png \
    images/icons/cil-battery-alert.png \
    images/icons/cil-battery-slash.png \
    images/icons/cil-battery-slash.png \
    images/icons/cil-bell.png \
    images/icons/cil-bell.png \
    images/icons/cil-bluetooth.png \
    images/icons/cil-bluetooth.png \
    images/icons/cil-briefcase.png \
    images/icons/cil-briefcase.png \
    images/icons/cil-browser.png \
    images/icons/cil-browser.png \
    images/icons/cil-calendar-check.png \
    images/icons/cil-calendar-check.png \
    images/icons/cil-camera-roll.png \
    images/icons/cil-camera-roll.png \
    images/icons/cil-camera.png \
    images/icons/cil-camera.png \
    images/icons/cil-caret-bottom.png \
    images/icons/cil-caret-bottom.png \
    images/icons/cil-caret-left.png \
    images/icons/cil-caret-left.png \
    images/icons/cil-caret-right.png \
    images/icons/cil-caret-right.png \
    images/icons/cil-caret-top.png \
    images/icons/cil-caret-top.png \
    images/icons/cil-cart.png \
    images/icons/cil-cart.png \
    images/icons/cil-cast.png \
    images/icons/cil-cast.png \
    images/icons/cil-chart-line.png \
    images/icons/cil-chart-line.png \
    images/icons/cil-chart-pie.png \
    images/icons/cil-chart-pie.png \
    images/icons/cil-chart.png \
    images/icons/cil-chart.png \
    images/icons/cil-chat-bubble.png \
    images/icons/cil-chat-bubble.png \
    images/icons/cil-check-alt.png \
    images/icons/cil-check-alt.png \
    images/icons/cil-check-circle.png \
    images/icons/cil-check-circle.png \
    images/icons/cil-check.png \
    images/icons/cil-check.png \
    images/icons/cil-chevron-bottom.png \
    images/icons/cil-chevron-bottom.png \
    images/icons/cil-chevron-circle-down-alt.png \
    images/icons/cil-chevron-circle-down-alt.png \
    images/icons/cil-chevron-circle-left-alt.png \
    images/icons/cil-chevron-circle-left-alt.png \
    images/icons/cil-chevron-circle-right-alt.png \
    images/icons/cil-chevron-circle-right-alt.png \
    images/icons/cil-chevron-circle-up-alt.png \
    images/icons/cil-chevron-circle-up-alt.png \
    images/icons/cil-chevron-double-down.png \
    images/icons/cil-chevron-double-down.png \
    images/icons/cil-chevron-double-left.png \
    images/icons/cil-chevron-double-left.png \
    images/icons/cil-chevron-double-right.png \
    images/icons/cil-chevron-double-right.png \
    images/icons/cil-chevron-double-up-alt.png \
    images/icons/cil-chevron-double-up-alt.png \
    images/icons/cil-chevron-double-up.png \
    images/icons/cil-chevron-double-up.png \
    images/icons/cil-chevron-left.png \
    images/icons/cil-chevron-left.png \
    images/icons/cil-chevron-right.png \
    images/icons/cil-chevron-right.png \
    images/icons/cil-chevron-top.png \
    images/icons/cil-chevron-top.png \
    images/icons/cil-circle.png \
    images/icons/cil-circle.png \
    images/icons/cil-clipboard.png \
    images/icons/cil-clipboard.png \
    images/icons/cil-clock.png \
    images/icons/cil-clock.png \
    images/icons/cil-clone.png \
    images/icons/cil-clone.png \
    images/icons/cil-closed-captioning.png \
    images/icons/cil-closed-captioning.png \
    images/icons/cil-cloud-download.png \
    images/icons/cil-cloud-download.png \
    images/icons/cil-cloud-upload.png \
    images/icons/cil-cloud-upload.png \
    images/icons/cil-cloudy.png \
    images/icons/cil-cloudy.png \
    images/icons/cil-code.png \
    images/icons/cil-code.png \
    images/icons/cil-coffee.png \
    images/icons/cil-coffee.png \
    images/icons/cil-comment-bubble.png \
    images/icons/cil-comment-bubble.png \
    images/icons/cil-comment-square.png \
    images/icons/cil-comment-square.png \
    images/icons/cil-credit-card.png \
    images/icons/cil-credit-card.png \
    images/icons/cil-cursor-move.png \
    images/icons/cil-cursor-move.png \
    images/icons/cil-cursor.png \
    images/icons/cil-cursor.png \
    images/icons/cil-cut.png \
    images/icons/cil-cut.png \
    images/icons/cil-data-transfer-down.png \
    images/icons/cil-data-transfer-down.png \
    images/icons/cil-data-transfer-up.png \
    images/icons/cil-data-transfer-up.png \
    images/icons/cil-deaf.png \
    images/icons/cil-deaf.png \
    images/icons/cil-description.png \
    images/icons/cil-description.png \
    images/icons/cil-devices.png \
    images/icons/cil-devices.png \
    images/icons/cil-dialpad.png \
    images/icons/cil-dialpad.png \
    images/icons/cil-dog.png \
    images/icons/cil-dog.png \
    images/icons/cil-door.png \
    images/icons/cil-door.png \
    images/icons/cil-double-quote-sans-left.png \
    images/icons/cil-double-quote-sans-left.png \
    images/icons/cil-double-quote-sans-right.png \
    images/icons/cil-double-quote-sans-right.png \
    images/icons/cil-drop.png \
    images/icons/cil-drop.png \
    images/icons/cil-envelope-closed.png \
    images/icons/cil-envelope-closed.png \
    images/icons/cil-envelope-letter.png \
    images/icons/cil-envelope-letter.png \
    images/icons/cil-envelope-open.png \
    images/icons/cil-envelope-open.png \
    images/icons/cil-equalizer.png \
    images/icons/cil-equalizer.png \
    images/icons/cil-ethernet.png \
    images/icons/cil-ethernet.png \
    images/icons/cil-exit-to-app.png \
    images/icons/cil-exit-to-app.png \
    images/icons/cil-expand-down.png \
    images/icons/cil-expand-down.png \
    images/icons/cil-expand-left.png \
    images/icons/cil-expand-left.png \
    images/icons/cil-expand-right.png \
    images/icons/cil-expand-right.png \
    images/icons/cil-expand-up.png \
    images/icons/cil-expand-up.png \
    images/icons/cil-exposure.png \
    images/icons/cil-exposure.png \
    images/icons/cil-external-link.png \
    images/icons/cil-external-link.png \
    images/icons/cil-face-dead.png \
    images/icons/cil-face-dead.png \
    images/icons/cil-featured-playlist.png \
    images/icons/cil-featured-playlist.png \
    images/icons/cil-file.png \
    images/icons/cil-file.png \
    images/icons/cil-find-in-page.png \
    images/icons/cil-find-in-page.png \
    images/icons/cil-fingerprint.png \
    images/icons/cil-fingerprint.png \
    images/icons/cil-fire.png \
    images/icons/cil-fire.png \
    images/icons/cil-flip-to-back.png \
    images/icons/cil-flip-to-back.png \
    images/icons/cil-folder-open.png \
    images/icons/cil-folder-open.png \
    images/icons/cil-folder.png \
    images/icons/cil-folder.png \
    images/icons/cil-frown.png \
    images/icons/cil-frown.png \
    images/icons/cil-gamepad.png \
    images/icons/cil-gamepad.png \
    images/icons/cil-hand-point-down.png \
    images/icons/cil-hand-point-down.png \
    images/icons/cil-hand-point-left.png \
    images/icons/cil-hand-point-left.png \
    images/icons/cil-hand-point-right.png \
    images/icons/cil-hand-point-right.png \
    images/icons/cil-hand-point-up.png \
    images/icons/cil-hand-point-up.png \
    images/icons/cil-hd.png \
    images/icons/cil-hd.png \
    images/icons/cil-hdr.png \
    images/icons/cil-hdr.png \
    images/icons/cil-headphones.png \
    images/icons/cil-headphones.png \
    images/icons/cil-heart.png \
    images/icons/cil-heart.png \
    images/icons/cil-highligt.png \
    images/icons/cil-highligt.png \
    images/icons/cil-history.png \
    images/icons/cil-history.png \
    images/icons/cil-home.png \
    images/icons/cil-home.png \
    images/icons/cil-house.png \
    images/icons/cil-house.png \
    images/icons/cil-image-plus.png \
    images/icons/cil-image-plus.png \
    images/icons/cil-image1.png \
    images/icons/cil-image1.png \
    images/icons/cil-infinity.png \
    images/icons/cil-infinity.png \
    images/icons/cil-input-power.png \
    images/icons/cil-input-power.png \
    images/icons/cil-input.png \
    images/icons/cil-input.png \
    images/icons/cil-justify-center.png \
    images/icons/cil-justify-center.png \
    images/icons/cil-justify-left.png \
    images/icons/cil-justify-left.png \
    images/icons/cil-justify-right.png \
    images/icons/cil-justify-right.png \
    images/icons/cil-keyboard.png \
    images/icons/cil-keyboard.png \
    images/icons/cil-laptop.png \
    images/icons/cil-laptop.png \
    images/icons/cil-layers.png \
    images/icons/cil-layers.png \
    images/icons/cil-level-down.png \
    images/icons/cil-level-down.png \
    images/icons/cil-level-up.png \
    images/icons/cil-level-up.png \
    images/icons/cil-library-add.png \
    images/icons/cil-library-add.png \
    images/icons/cil-library.png \
    images/icons/cil-library.png \
    images/icons/cil-lightbulb.png \
    images/icons/cil-lightbulb.png \
    images/icons/cil-link-alt.png \
    images/icons/cil-link-alt.png \
    images/icons/cil-link-broken.png \
    images/icons/cil-link-broken.png \
    images/icons/cil-link.png \
    images/icons/cil-link.png \
    images/icons/cil-location-pin.png \
    images/icons/cil-location-pin.png \
    images/icons/cil-lock-locked.png \
    images/icons/cil-lock-locked.png \
    images/icons/cil-lock-unlocked.png \
    images/icons/cil-lock-unlocked.png \
    images/icons/cil-loop-1.png \
    images/icons/cil-loop-1.png \
    images/icons/cil-loop-circular.png \
    images/icons/cil-loop-circular.png \
    images/icons/cil-loop.png \
    images/icons/cil-loop.png \
    images/icons/cil-low-vision.png \
    images/icons/cil-low-vision.png \
    images/icons/cil-magnifying-glass.png \
    images/icons/cil-magnifying-glass.png \
    images/icons/cil-map.png \
    images/icons/cil-map.png \
    images/icons/cil-media-eject.png \
    images/icons/cil-media-eject.png \
    images/icons/cil-media-pause.png \
    images/icons/cil-media-pause.png \
    images/icons/cil-media-play.png \
    images/icons/cil-media-play.png \
    images/icons/cil-media-skip-backward.png \
    images/icons/cil-media-skip-backward.png \
    images/icons/cil-media-skip-forward.png \
    images/icons/cil-media-skip-forward.png \
    images/icons/cil-media-step-backward.png \
    images/icons/cil-media-step-backward.png \
    images/icons/cil-media-step-forward.png \
    images/icons/cil-media-step-forward.png \
    images/icons/cil-media-stop.png \
    images/icons/cil-media-stop.png \
    images/icons/cil-medical-cross.png \
    images/icons/cil-medical-cross.png \
    images/icons/cil-meh.png \
    images/icons/cil-meh.png \
    images/icons/cil-menu.png \
    images/icons/cil-menu.png \
    images/icons/cil-microphone.png \
    images/icons/cil-microphone.png \
    images/icons/cil-minus.png \
    images/icons/cil-minus.png \
    images/icons/cil-mobile-landscape.png \
    images/icons/cil-mobile-landscape.png \
    images/icons/cil-mobile.png \
    images/icons/cil-mobile.png \
    images/icons/cil-mood-bad.png \
    images/icons/cil-mood-bad.png \
    images/icons/cil-mood-good.png \
    images/icons/cil-mood-good.png \
    images/icons/cil-mood-very-bad.png \
    images/icons/cil-mood-very-bad.png \
    images/icons/cil-mood-very-good.png \
    images/icons/cil-mood-very-good.png \
    images/icons/cil-moon.png \
    images/icons/cil-moon.png \
    images/icons/cil-mouse.png \
    images/icons/cil-mouse.png \
    images/icons/cil-move.png \
    images/icons/cil-move.png \
    images/icons/cil-movie.png \
    images/icons/cil-movie.png \
    images/icons/cil-mug-tea.png \
    images/icons/cil-mug-tea.png \
    images/icons/cil-mug.png \
    images/icons/cil-mug.png \
    images/icons/cil-notes.png \
    images/icons/cil-notes.png \
    images/icons/cil-options-horizontal.png \
    images/icons/cil-options-horizontal.png \
    images/icons/cil-options.png \
    images/icons/cil-options.png \
    images/icons/cil-paint-bucket.png \
    images/icons/cil-paint-bucket.png \
    images/icons/cil-paper-plane.png \
    images/icons/cil-paper-plane.png \
    images/icons/cil-paperclip.png \
    images/icons/cil-paperclip.png \
    images/icons/cil-paragraph.png \
    images/icons/cil-paragraph.png \
    images/icons/cil-pen-alt.png \
    images/icons/cil-pen-alt.png \
    images/icons/cil-pencil.png \
    images/icons/cil-pencil.png \
    images/icons/cil-people.png \
    images/icons/cil-people.png \
    images/icons/cil-phone.png \
    images/icons/cil-phone.png \
    images/icons/cil-pin.png \
    images/icons/cil-pin.png \
    images/icons/cil-plus.png \
    images/icons/cil-plus.png \
    images/icons/cil-power-standby.png \
    images/icons/cil-power-standby.png \
    images/icons/cil-print.png \
    images/icons/cil-print.png \
    images/icons/cil-rectangle.png \
    images/icons/cil-rectangle.png \
    images/icons/cil-reload.png \
    images/icons/cil-reload.png \
    images/icons/cil-remove.png \
    images/icons/cil-remove.png \
    images/icons/cil-rss.png \
    images/icons/cil-rss.png \
    images/icons/cil-satelite.png \
    images/icons/cil-satelite.png \
    images/icons/cil-save.png \
    images/icons/cil-save.png \
    images/icons/cil-screen-desktop.png \
    images/icons/cil-screen-desktop.png \
    images/icons/cil-screen-smartphone.png \
    images/icons/cil-screen-smartphone.png \
    images/icons/cil-settings.png \
    images/icons/cil-settings.png \
    images/icons/cil-share-boxed.png \
    images/icons/cil-share-boxed.png \
    images/icons/cil-share.png \
    images/icons/cil-share.png \
    images/icons/cil-signal-cellular-0.png \
    images/icons/cil-signal-cellular-0.png \
    images/icons/cil-signal-cellular-3.png \
    images/icons/cil-signal-cellular-3.png \
    images/icons/cil-size-grip.png \
    images/icons/cil-size-grip.png \
    images/icons/cil-smile.png \
    images/icons/cil-smile.png \
    images/icons/cil-speaker.png \
    images/icons/cil-speaker.png \
    images/icons/cil-speech.png \
    images/icons/cil-speech.png \
    images/icons/cil-speedometer.png \
    images/icons/cil-speedometer.png \
    images/icons/cil-star.png \
    images/icons/cil-star.png \
    images/icons/cil-tags.png \
    images/icons/cil-tags.png \
    images/icons/cil-task.png \
    images/icons/cil-task.png \
    images/icons/cil-terminal.png \
    images/icons/cil-terminal.png \
    images/icons/cil-text-size.png \
    images/icons/cil-text-size.png \
    images/icons/cil-text-square.png \
    images/icons/cil-text-square.png \
    images/icons/cil-text.png \
    images/icons/cil-text.png \
    images/icons/cil-thumb-down.png \
    images/icons/cil-thumb-down.png \
    images/icons/cil-thumb-up.png \
    images/icons/cil-thumb-up.png \
    images/icons/cil-transfer.png \
    images/icons/cil-transfer.png \
    images/icons/cil-triangle.png \
    images/icons/cil-triangle.png \
    images/icons/cil-truck.png \
    images/icons/cil-truck.png \
    images/icons/cil-user-female.png \
    images/icons/cil-user-female.png \
    images/icons/cil-user-follow.png \
    images/icons/cil-user-follow.png \
    images/icons/cil-user-unfollow.png \
    images/icons/cil-user-unfollow.png \
    images/icons/cil-user.png \
    images/icons/cil-user.png \
    images/icons/cil-vertical-align-bottom.png \
    images/icons/cil-vertical-align-bottom.png \
    images/icons/cil-view-column.png \
    images/icons/cil-view-column.png \
    images/icons/cil-view-module.png \
    images/icons/cil-view-module.png \
    images/icons/cil-view-quilt.png \
    images/icons/cil-view-quilt.png \
    images/icons/cil-view-stream.png \
    images/icons/cil-view-stream.png \
    images/icons/cil-voice-over-record.png \
    images/icons/cil-voice-over-record.png \
    images/icons/cil-volume-high.png \
    images/icons/cil-volume-high.png \
    images/icons/cil-volume-low.png \
    images/icons/cil-volume-low.png \
    images/icons/cil-volume-off.png \
    images/icons/cil-volume-off.png \
    images/icons/cil-wallet.png \
    images/icons/cil-wallet.png \
    images/icons/cil-watch.png \
    images/icons/cil-watch.png \
    images/icons/cil-wifi-signal-0.png \
    images/icons/cil-wifi-signal-0.png \
    images/icons/cil-wifi-signal-1.png \
    images/icons/cil-wifi-signal-1.png \
    images/icons/cil-wifi-signal-2.png \
    images/icons/cil-wifi-signal-2.png \
    images/icons/cil-wifi-signal-4.png \
    images/icons/cil-wifi-signal-4.png \
    images/icons/cil-wifi-signal-off.png \
    images/icons/cil-wifi-signal-off.png \
    images/icons/cil-window-maximize.png \
    images/icons/cil-window-maximize.png \
    images/icons/cil-window-minimize.png \
    images/icons/cil-window-minimize.png \
    images/icons/cil-window-restore.png \
    images/icons/cil-window-restore.png \
    images/icons/cil-wrap-text.png \
    images/icons/cil-wrap-text.png \
    images/icons/cil-x-circle.png \
    images/icons/cil-x-circle.png \
    images/icons/cil-x.png \
    images/icons/cil-x.png \
    images/icons/cil-zoom-in.png \
    images/icons/cil-zoom-in.png \
    images/icons/cil-zoom-out.png \
    images/icons/cil-zoom-out.png \
    images/icons/icon_close.png \
    images/icons/icon_close.png \
    images/icons/icon_maximize.png \
    images/icons/icon_maximize.png \
    images/icons/icon_menu.png \
    images/icons/icon_menu.png \
    images/icons/icon_minimize.png \
    images/icons/icon_minimize.png \
    images/icons/icon_restore.png \
    images/icons/icon_restore.png \
    images/icons/icon_settings.png \
    images/icons/icon_settings.png \
    images/images/PyDracula.png \
    images/images/PyDracula.png \
    images/images/PyDracula_horizontal.png \
    images/images/PyDracula_horizontal.png \
    images/images/PyDracula_vertical.png \
    images/images/PyDracula_vertical.png \
    modules/__init__.py \
    modules/__init__.py \
    modules/app_functions.py \
    modules/app_functions.py \
    modules/app_settings.py \
    modules/app_settings.py \
    modules/resources_rc.py \
    modules/resources_rc.py \
    modules/ui_functions.py \
    modules/ui_functions.py \
    modules/ui_main.py \
    modules/ui_main.py \
    themes/py_dracula_dark.qss \
    themes/py_dracula_dark.qss \
    themes/py_dracula_light.qss \
    themes/py_dracula_light.qss \
    widgets/__init__.py \
    widgets/__init__.py \
    widgets/custom_grips/__init__.py \
    widgets/custom_grips/__init__.py \
    widgets/custom_grips/custom_grips.py \
    widgets/custom_grips/custom_grips.py
