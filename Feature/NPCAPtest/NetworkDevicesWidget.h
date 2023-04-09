#ifndef NETWORKDEVICESWIDGET_H
#define NETWORKDEVICESWIDGET_H
#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>
#include <WinSock2.h>
#include <windows.h>
#include <Iphlpapi.h>
#include <sstream>  // for std::stringstream
#include <iomanip>  // for std::setw and std::setfill
#include <codecvt>  // for std::wstring_convert
#include <locale>   // for std::locale
#include <BluetoothAPIs.h>

#include <algorithm>


#include <regex>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")
class NetworkDevicesWidget : public QWidget {
    Q_OBJECT
public:
    explicit NetworkDevicesWidget(QWidget *parent = nullptr) : QWidget(parent) {
        QLabel *label = new QLabel("Select network interface:");
        comboBox = new QComboBox();
        QLabel *macAddressLabel = new QLabel("Hardware Address:");
        macAddressValueLabel = new QLabel();
        QLabel *ipLabel = new QLabel("IP:");
        ipValueLabel = new QLabel();
        QLabel *subnetMaskLabel = new QLabel("Subnet Mask:");
        subnetMaskValueLabel = new QLabel();
        QLabel *gatewayLabel = new QLabel("Gateway:");
        gatewayValueLabel = new QLabel();

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(label);
        layout->addWidget(comboBox);
        layout->addWidget(macAddressLabel);
        layout->addWidget(macAddressValueLabel);
        layout->addWidget(ipLabel);
        layout->addWidget(ipValueLabel);
        layout->addWidget(subnetMaskLabel);
        layout->addWidget(subnetMaskValueLabel);
        layout->addWidget(gatewayLabel);
        layout->addWidget(gatewayValueLabel);
        setLayout(layout);

        updateDeviceList();
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDeviceInfo(int)));
    }

private slots:
    void updateDeviceInfo(int index) {
        PIP_ADAPTER_INFO adapterInfo = adapterInfoList;
        for (int i = 0; i < index; i++) {
            adapterInfo = adapterInfo->Next;
        }


        std::stringstream macAddressStream;
        for (int i = 0; i < adapterInfo->AddressLength; i++) {
            macAddressStream << std::hex << std::setw(2) << std::setfill('0') << (int)adapterInfo->Address[i];
            if (i < adapterInfo->AddressLength - 1) {
                macAddressStream << ":";
            }
        }

        std::string macAddress = macAddressStream.str();
        macAddressValueLabel->setText(QString::fromStdString(macAddress));
        ipValueLabel->setText(QString::fromStdString(adapterInfo->IpAddressList.IpAddress.String));
        subnetMaskValueLabel->setText(QString::fromStdString(adapterInfo->IpAddressList.IpMask.String));
        gatewayValueLabel->setText(QString::fromStdString(adapterInfo->GatewayList.IpAddress.String));
      //authAlgorithmValueLabel->setText(QString::fromStdString(adapterInfo->AuthAlgorithm.AlgorithmName));



        // Get adapter type
            QString adapterType;
            switch (adapterInfo->Type)
            {
            case IF_TYPE_SOFTWARE_LOOPBACK:
                adapterType = "Virtual";
                break;
                case MIB_IF_TYPE_ETHERNET:
                    adapterType = "Ethernet";
                    break;
                case IF_TYPE_IEEE80211:
                    adapterType = "Wi-Fi";
                    break;
                /*case IF_TYPE_BLUETOOTH:
                    adapterType = "Bluetooth";
                    break;
                case IF_TYPE_CELLULAR:
                    adapterType = "Cellular";
                    break;*/

                case IF_TYPE_PPP:
                    adapterType = "VPN";
                    break;
                default:
                    adapterType = "Unknown";
                    break;
            }

            qDebug() << adapterType;
            const std::string keyword = "Virtual";
            if ((  std::string(adapterInfo->Description).find(keyword) != std::wstring::npos))
                        {
                            qDebug() << (adapterInfo->Description) << " contains the keyword " << QString::fromStdString(keyword) <<'\n';
                        }


        qDebug() << '\n' << QString::fromStdString(GetAdapterStatus(std::string(adapterInfo->AdapterName)));
        //qDebug() << '\n'<<"Bluetooth is " << isBluetoothDeviceConnected(adapterInfo->Description);


    }
    const wchar_t *GetWC(const char *c)
    {
        const size_t cSize = strlen(c)+1;
        wchar_t* wc = new wchar_t[cSize];
        mbstowcs (wc, c, cSize);

        return wc;
    }
    // Function to extract GUID from network device string
    std::string extractGuid(const std::string& str)
    {
       std::regex re("\\{(.*)\\}");
        std::smatch match;
        std::string guid;

        if (std::regex_search(str, match, re))
        {
            guid = match[1].str();
        }

        return '{' +  guid + '}';
    }

    std::string GetAdapterStatus(const std::string& adapterName)
    {
        PIP_ADAPTER_ADDRESSES adapterAddresses = nullptr;
        ULONG bufferSize = 0;
        ULONG result = GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, adapterAddresses, &bufferSize);
        adapterAddresses = static_cast<PIP_ADAPTER_ADDRESSES>(malloc(bufferSize));
        result = GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, adapterAddresses, &bufferSize);

        if (result != NO_ERROR)
        {
            free(adapterAddresses);
            //std::cerr << "Error getting adapter addresses: " << result << std::endl;
            return "Error";
        }


        PIP_ADAPTER_ADDRESSES currentAdapter = adapterAddresses;
        while (currentAdapter)
        {
              //qDebug() << "Friendly name "<<  currentAdapter->AdapterName << " and " << QString::fromStdString(adapterName) <<'\n';
            if (adapterName == currentAdapter->AdapterName)
            {
                if (currentAdapter->OperStatus == IfOperStatusUp)
                {
                    free(adapterAddresses);
                    return "Connected";
                }
                else
                {
                    free(adapterAddresses);
                    return "Disconnected";
                }
            }
            currentAdapter = currentAdapter->Next;
        }

        free(adapterAddresses);

        return "Adapter not found";
    }



private:
    QComboBox *comboBox;
    QLabel *macAddressValueLabel;
    QLabel *ipValueLabel;
    QLabel *subnetMaskValueLabel;
    QLabel *gatewayValueLabel;
    PIP_ADAPTER_INFO adapterInfoList;

    void updateDeviceList()
    {
        adapterInfoList = new IP_ADAPTER_INFO;
        ULONG bufferSize = sizeof(IP_ADAPTER_INFO);
        DWORD result = GetAdaptersInfo(adapterInfoList, &bufferSize);
        if (result == ERROR_BUFFER_OVERFLOW) {
            delete adapterInfoList;
            adapterInfoList = (PIP_ADAPTER_INFO) new BYTE[bufferSize];
            result = GetAdaptersInfo(adapterInfoList, &bufferSize);
        }
        if (result == NO_ERROR) {
            PIP_ADAPTER_INFO adapterInfo = adapterInfoList;
            while (adapterInfo) {
                comboBox->addItem(QString::fromStdString(adapterInfo->Description));
                adapterInfo = adapterInfo->Next;
            }
        }


    }

    bool isBluetoothDeviceConnected(const std::string& deviceName)
    {
        BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams = { sizeof(searchParams) };
        searchParams.fIssueInquiry = TRUE;
             searchParams.fReturnAuthenticated = TRUE;
             searchParams.fReturnConnected = TRUE;
             searchParams.fReturnRemembered = FALSE;
             searchParams.fReturnUnknown = TRUE;
        BLUETOOTH_DEVICE_INFO btDeviceInfo = { sizeof(btDeviceInfo) };
        DWORD searchHandle = BluetoothGetDeviceInfo(&searchParams, &btDeviceInfo);

        if (searchHandle == ERROR_NOT_FOUND)
        {
            qDebug() << "Hi from exit <";
            // Failed to start device search
            return false;
        }

        bool deviceFound = false;

        do {
            btDeviceInfo.dwSize = sizeof(btDeviceInfo);
            std::wstring wideName(btDeviceInfo.szName);
            std::string deviceNameUtf8 = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wideName);
            qDebug() <<
                     " blue is "<<QString::fromStdString(deviceNameUtf8) << '\n';
            if (deviceName == deviceNameUtf8)
            {
                deviceFound = true;

                if (btDeviceInfo.fConnected)
                {
                    // Device is connected
                    return true;
                }
                else
                {
                    // Device is not connected
                    return false;
                }
            }
        } while (BluetoothFindNextDevice(&searchHandle, &btDeviceInfo) && !deviceFound);

        // Device not found
        return false;
    }

};
#endif // NETWORKDEVICESWIDGET_H
