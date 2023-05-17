// #ifndef NETWORKDEVICESWIDGET_H
// #define NETWORKDEVICESWIDGET_H
// #include <QComboBox>
// #include <QVBoxLayout>
// #include <QLabel>
// #include <QWidget>
// #include <WinSock2.h>
// #include <ws2tcpip.h>
// #include <windows.h>
// #include <Iphlpapi.h>
// #include <sstream>  // for std::stringstream
// #include <iomanip>  // for std::setw and std::setfill
// #include <codecvt>  // for std::wstring_convert
// #include <locale>   // for std::locale
// #include <BluetoothAPIs.h>
// #include <QTimer>
// #include <algorithm>
// #include <wlanapi.h>

// #include <regex>
// #pragma comment(lib, "Wlanapi.lib")
// #pragma comment(lib, "Ws2_32.lib")
// #pragma comment(lib, "IPHLPAPI.lib")
// class NetworkDevicesWidget : public QWidget
//{
//	Q_OBJECT

// public:
//	explicit NetworkDevicesWidget(QWidget* parent = nullptr) : QWidget(parent)
//	{
//		QLabel* label = new QLabel("Select network interface:");
//		comboBox = new QComboBox();
//		QLabel* macAddressLabel = new QLabel("Hardware Address:");
//		macAddressValueLabel = new QLabel();
//		QLabel* ipLabel = new QLabel("IP:");
//		ipValueLabel = new QLabel();
//		QLabel* subnetMaskLabel = new QLabel("Subnet Mask:");
//		subnetMaskValueLabel = new QLabel();
//		QLabel* gatewayLabel = new QLabel("Gateway:");
//		gatewayValueLabel = new QLabel();
//		QLabel* sUpLabel = new QLabel("Download:");
//		upSpeed = new QLabel();
//		QLabel* sDwLabel = new QLabel("Upload:");
//		downSpeed = new QLabel();
//		preInBytes = preOutBytes = inBytes = outBytes = 0;
//		QVBoxLayout* layout = new QVBoxLayout();
//		layout->addWidget(label);
//		layout->addWidget(comboBox);
//		layout->addWidget(macAddressLabel);
//		layout->addWidget(macAddressValueLabel);
//		layout->addWidget(ipLabel);
//		layout->addWidget(ipValueLabel);
//		layout->addWidget(subnetMaskLabel);
//		layout->addWidget(subnetMaskValueLabel);
//		layout->addWidget(gatewayLabel);
//		layout->addWidget(gatewayValueLabel);
//		layout->addWidget(sUpLabel);
//		layout->addWidget(upSpeed);
//		layout->addWidget(sDwLabel);
//		layout->addWidget(downSpeed);
//		setLayout(layout);

//		updateDeviceList();
//		connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDeviceInfo(int)));
//	}

// private slots:
//	void updateDeviceInfo(int index)
//	{
//                 qDebug("Index is %d", index);
//		disconnect(timer, &QTimer::timeout, this, &NetworkDevicesWidget::UpdateSpeed);
//		adapterInfo = adapterInfoList;
//		for (int i = 0; i < index; i++)
//		{
//			adapterInfo = adapterInfo->Next;
//		}
//         qDebug("Adapter Index is %d", adapterInfo->Index);

//		preInBytes = preOutBytes = inBytes = outBytes = 0;

//	//DWORD Size = sizeof(MIB_IFTABLE);
//	//MIB_IFTABLE* Table = (PMIB_IFTABLE)malloc(Size);
//	//Size = sizeof(MIB_IFTABLE);
//	//
//	//while (GetIfTable(Table, &Size,TRUE) == ERROR_INSUFFICIENT_BUFFER)
//	//{
//	//	free(Table);
//	//	Table = (PMIB_IFTABLE)malloc(Size);
//	//}
//	//
//	//for (DWORD i = 0; i < Table->dwNumEntries; i++)
//	//{
//	//	if (Table->table[i].dwOperStatus != MIB_IF_OPER_STATUS_CONNECTED &&
//	//		Table->table[i].dwOperStatus != IF_OPER_STATUS_OPERATIONAL)
//	//	{
//	//		continue;
//	//	}
//	//
//	//	inBytes += Table->table[i].dwInOctets;
//	//	outBytes += Table->table[i].dwOutOctets;
//	//}
//	//
//	//free(Table);

//		connect(timer, &QTimer::timeout, this, &NetworkDevicesWidget::UpdateSpeed);
//		timer->start(1000);

//		std::stringstream macAddressStream;
//		for (int i = 0; i < adapterInfo->AddressLength; i++)
//		{
//			macAddressStream << std::hex << std::setw(2) << std::setfill('0') << (int)adapterInfo->Address[i];
//			if (i < adapterInfo->AddressLength - 1)
//			{
//				macAddressStream << ":";
//			}
//		}

//		std::string macAddress = macAddressStream.str();
//		macAddressValueLabel->setText(QString::fromStdString(macAddress));
//		ipValueLabel->setText(QString::fromStdString(adapterInfo->IpAddressList.IpAddress.String));
//		subnetMaskValueLabel->setText(QString::fromStdString(adapterInfo->IpAddressList.IpMask.String));
//		gatewayValueLabel->setText(QString::fromStdString(adapterInfo->GatewayList.IpAddress.String));

//		// Get adapter type
//		QString adapterType;
//		switch (adapterInfo->Type)
//		{
//		case IF_TYPE_SOFTWARE_LOOPBACK:
//			adapterType = "Virtual";
//			break;
//		case MIB_IF_TYPE_ETHERNET:
//			adapterType = "Ethernet";
//			break;
//		case IF_TYPE_IEEE80211:
//			adapterType = "Wi-Fi";
//			break;
//		case IF_TYPE_PPP:
//			adapterType = "VPN";
//			break;
//		default:
//			adapterType = "Unknown";
//			break;
//		}

//		qDebug() << adapterType;
//		const std::string keyword = "Virtual";
//		if ((std::string(adapterInfo->Description).find(keyword) != std::wstring::npos))
//		{
//			qDebug() << (adapterInfo->Description) << " contains the keyword " << QString::fromStdString(keyword) <<
//				'\n';
//		}
//		qDebug() << '\n' << QString::fromStdString(GetAdapterStatus(std::string(adapterInfo->AdapterName)));
//	}

//	const wchar_t* GetWC(const char* c)
//	{
//		const size_t cSize = strlen(c) + 1;
//		wchar_t* wc = new wchar_t[cSize];
//		mbstowcs(wc, c, cSize);

//		return wc;
//	}

//	// Function to extract GUID from network device string
//	std::string extractGuid(const std::string& str)
//	{
//		std::regex re("\\{(.*)\\}");
//		std::smatch match;
//		std::string guid;

//		if (std::regex_search(str, match, re))
//		{
//			guid = match[1].str();
//		}

//		return '{' + guid + '}';
//	}

//	std::string GetAdapterStatus(const std::string& adapterName)
//	{
//		PIP_ADAPTER_ADDRESSES adapterAddresses = nullptr;
//		ULONG bufferSize = 0;
//		ULONG result = GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, adapterAddresses, &bufferSize);
//		adapterAddresses = static_cast<PIP_ADAPTER_ADDRESSES>(malloc(bufferSize));
//		result = GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, adapterAddresses, &bufferSize);

//		if (result != NO_ERROR)
//		{
//			free(adapterAddresses);
//			//std::cerr << "Error getting adapter addresses: " << result << std::endl;
//			return "Error";
//		}

//		PIP_ADAPTER_ADDRESSES currentAdapter = adapterAddresses;
//		while (currentAdapter)
//		{
//			//qDebug() << "Friendly name "<<  currentAdapter->AdapterName << " and " << QString::fromStdString(adapterName)
//<<'\n'; 			if (adapterName == currentAdapter->AdapterName)
//			{
//				if (currentAdapter->OperStatus == IfOperStatusUp)
//				{
//					free(adapterAddresses);
//					return "Connected";
//				}
//				else
//				{
//					free(adapterAddresses);
//					return "Disconnected";
//				}
//			}
//			currentAdapter = currentAdapter->Next;
//		}

//		free(adapterAddresses);

//		return "Adapter not found";
//	}

//	// signals:
//	//    void valueNetworkTrafficChanged(int downSpeed, int upSpeed);
//	//
// public slots:
//	void UpdateSpeed()
//	{
//		DWORD Size = sizeof(MIB_IFTABLE);
//		MIB_IFTABLE* Table = (PMIB_IFTABLE)malloc(Size);

//		DWORD Size2 = sizeof(MIB_IF_TABLE2);
//        MIB_IF_TABLE2 *t2 = (PMIB_IF_TABLE2)malloc(Size2);

//		Size2 = sizeof(MIB_IF_TABLE2);

//		Size = sizeof(MIB_IFTABLE);

//		while (GetIfTable2(&t2) == ERROR_NOT_ENOUGH_MEMORY)
//		{
//            free(t2);
//            t2 = (PMIB_IF_TABLE2)malloc(Size2);
//		}
//        qDebug() << "Table2 num is " << t2->NumEntries << '\n';

//		while (GetIfTable(Table, &Size,TRUE) == ERROR_INSUFFICIENT_BUFFER)
//		{
//			free(Table);
//			Table = (PMIB_IFTABLE)malloc(Size);
//		}

//		preInBytes = inBytes;
//		preOutBytes = outBytes;

//		inBytes = 0;
//		outBytes = 0;
//		MIB_IF_ROW2 ifRow = {};
//		ifRow.InterfaceIndex = adapterInfo->Index;
//		if (GetIfEntry2Ex(MibIfEntryNormal, &ifRow) == NO_ERROR){}

//		MIB_IFROW row1{};
//        row1.dwIndex = adapterInfo->Index;
//        if (GetIfEntry(&row1) == NO_ERROR)
//        {
//        }
//        inBytes += row1.dwInOctets;
//        outBytes += row1.dwOutOctets;

//		//for (DWORD i = 0; i < Table->dwNumEntries; i++)
//		//{
//		//
//		//	int cnt = 0;
//		//	//if (Table->table[i].dwOperStatus != MIB_IF_OPER_STATUS_CONNECTED &&
//		//	//	Table->table[i].dwOperStatus != IF_OPER_STATUS_OPERATIONAL)
//		//	//{
//		//	//	continue;
//		//	//}
//        //    qDebug() << "DwIndex is " << Table->table[i].dwIndex
//        //             << " MAC dw = " << Table->table[i].bPhysAddr << " MAC " << adapterInfo->Address << '\n';
//		//		// Check if the current interface index matches the selected adapter's index
//		//		//qDebug() << "index = " << Table->table[i].dwInOctets << "\t" << Table->table[adapterInfo->Index - 1].
//		//			//dwInOctets;
//		//	if (memcmp(Table->table[i].bPhysAddr, adapterInfo->Address, 6) == 0
//		//		|| (Table->table[i].dwType == Table->table[adapterInfo->Index-1].dwType))
//		//	{
//		//		//qDebug() << "Index dw = " << Table->table[i].dwIndex << " adapterInfo->Index " << adapterInfo->Index
//<<
//		//			//'\n';
//		//		//qDebug() << "Counter = " << cnt << " index is  " << Table->table[i].dwIndex <<  '\n';
//		//		//inBytes += Table->table[i].dwInOctets;
//		//		//outBytes += Table->table[i].dwOutOctets;
//		//	}
//		//		//qDebug() << "i = " << i << " ||  " << Table->table[i].dwInOctets << " || " << Table->table[i].bDescr << "
//|| "<<Table->table[i].dwIndex <<'\n';
//		//		// inBytes  += Table->table[i].dwInOctets;
//		//		// outBytes += Table->table[i].dwOutOctets;
//		//
//		//	   }
//		//free(Table);

//		qDebug("inBytes: %llu , outBytes: %llu\n", inBytes, outBytes);

//		this->setUpdatesEnabled(true);

//		QString text, str, text2;

//		float inKBs = 0;
//		float outKBs = 0;

//		if (((inBytes - preInBytes)) >= 8 * 1024 * 1024 ||
//			(outBytes - preOutBytes) >= 8 * 1024 * 1024)
//		{
//			inKBs = (inBytes - preInBytes) / (8 * 1024.0 * 1024);
//			outKBs = (outBytes - preOutBytes) / (8 * 1024.0 * 1024);

//			str.setNum(setPrecision(inKBs));
//			text = QLatin1String("In: ") + str + QLatin1String("MB/s ");
//			str.setNum(setPrecision(outKBs));
//			text2 = QLatin1String("Out: ") + str + QLatin1String("MB/s");
//		}
//		else if (((inBytes - preInBytes)) >= 8 * 1024 ||
//			(outBytes - preOutBytes) >= 8 * 1024)
//		{
//			inKBs = (inBytes - preInBytes) / (8 * 1024.0);
//			outKBs = (outBytes - preOutBytes) / (8 * 1024.0);

//			str.setNum(setPrecision(inKBs));
//			text = QLatin1String("In: ") + str + QLatin1String("KB/s ");
//			str.setNum(setPrecision(outKBs));
//			text2 = QLatin1String("Out: ") + str + QLatin1String("KB/s");

//		}
//		else
//		{
//			inKBs = (inBytes - preInBytes) / 8;
//			outKBs = (outBytes - preOutBytes) / 8;
//			str.setNum(inKBs);
//			text = QLatin1String("In: ") + str + QLatin1String("B/s ");
//			str.setNum(outKBs);
//			text2 = QLatin1String("Out: ") + str + QLatin1String("B/s");
//		}
//		qDebug("inBytes: %llu , outBytes: %llu\n", inBytes, preInBytes);

//		qDebug() << "inBytes - preInBytes" << inBytes - preInBytes << '\n';
//		downSpeed->setText(QString::number(row1.dwOutOctets) + " - " + text2);
//		upSpeed->setText(QString::number(row1.dwInOctets) + " - " + text);
//		qDebug() << text2 << '\n';
//	}

//	//
//	//    void setNettworkTraffic(int downSpeed, int upSpeed);
//	//

// private:
//	QComboBox* comboBox;
//	QLabel* macAddressValueLabel;
//	QLabel* ipValueLabel;
//	QLabel* subnetMaskValueLabel;
//	QLabel* gatewayValueLabel;
//	QLabel* upSpeed{};
//	QLabel* downSpeed{};
//	QLabel* bytesSend{};
//	QTimer* timer = new QTimer(this);
//	PIP_ADAPTER_INFO adapterInfoList;
//	PIP_ADAPTER_INFO adapterInfo;
//	unsigned __int64 inBytes;
//	unsigned __int64 outBytes;
//	unsigned __int64 preInBytes;
//	unsigned __int64 preOutBytes;

//	void updateDeviceList()
//	{
//		adapterInfoList = new IP_ADAPTER_INFO;
//		ULONG bufferSize = sizeof(IP_ADAPTER_INFO);
//		DWORD result = GetAdaptersInfo(adapterInfoList, &bufferSize);
//		if (result == ERROR_BUFFER_OVERFLOW)
//		{
//			delete adapterInfoList;
//			adapterInfoList = (PIP_ADAPTER_INFO)new BYTE[bufferSize];
//			result = GetAdaptersInfo(adapterInfoList, &bufferSize);
//		}
//		if (result == NO_ERROR)
//		{
//			PIP_ADAPTER_INFO adapterInfo = adapterInfoList;
//			while (adapterInfo)
//			{

//				comboBox->addItem(QString::fromStdString(adapterInfo->Description));
//				adapterInfo = adapterInfo->Next;
//			}
//		}
//	}

//	bool isBluetoothDeviceConnected(const std::string& deviceName)
//	{
//		BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams = {sizeof(searchParams)};
//		searchParams.fIssueInquiry = TRUE;
//		searchParams.fReturnAuthenticated = TRUE;
//		searchParams.fReturnConnected = TRUE;
//		searchParams.fReturnRemembered = FALSE;
//		searchParams.fReturnUnknown = TRUE;
//		BLUETOOTH_DEVICE_INFO btDeviceInfo = {sizeof(btDeviceInfo)};
//		DWORD searchHandle = BluetoothGetDeviceInfo(&searchParams, &btDeviceInfo);

//		if (searchHandle == ERROR_NOT_FOUND)
//		{
//			qDebug() << "Hi from exit <";
//			// Failed to start device search
//			return false;
//		}

//		bool deviceFound = false;

//		do
//		{
//			btDeviceInfo.dwSize = sizeof(btDeviceInfo);
//			std::wstring wideName(btDeviceInfo.szName);
//			std::string deviceNameUtf8 = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wideName);
//			qDebug() <<
//				" blue is " << QString::fromStdString(deviceNameUtf8) << '\n';
//			if (deviceName == deviceNameUtf8)
//			{
//				deviceFound = true;

//				if (btDeviceInfo.fConnected)
//				{
//					// Device is connected
//					return true;
//				}
//				else
//				{
//					// Device is not connected
//					return false;
//				}
//			}
//		}
//		while (BluetoothFindNextDevice(&searchHandle, &btDeviceInfo) && !deviceFound);

//		// Device not found
//		return false;
//	}

//	float setPrecision(const float v)
//	{
//        const int i = v * 10.0;
//		return i / 10.0;
//	}
//};
// #endif // NETWORKDEVICESWIDGET_H
