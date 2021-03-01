import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'main_page.dart';
import 'dart:typed_data';
import 'dart:convert';

bool isConnectedBT = false;

class BluetoothPage extends StatefulWidget {
  @override
  _BluetoothPageState createState() => _BluetoothPageState();
}

class _BluetoothPageState extends State<BluetoothPage>
    with WidgetsBindingObserver {
  BluetoothState _bluetoothState = BluetoothState.UNKNOWN;

  List<BluetoothDevice> myDevices = List<BluetoothDevice>();

  BluetoothConnection connectionBT;
  bool isConnectedBT = false;
  bool get isConnected => connectionBT != null && connectionBT.isConnected;
  bool isDisconnectedBT = true;

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addObserver(this);
    _getBTState();
    _statetChangeListener();
  }

  @override
  void dispose() {
    WidgetsBinding.instance.removeObserver(this);
    super.dispose();
  }

  @override
  void didChangeAppLifecycleState(AppLifecycleState state) {
    if (state.index == 0) {
      if (_bluetoothState.isEnabled) {
        _listBondedDevices();
      }
      setState(() {});
    }
  }

  _getBTState() {
    FlutterBluetoothSerial.instance.state.then((state) {
      _bluetoothState = state;
      if (_bluetoothState.isEnabled) {
        _listBondedDevices();
      }
    });
    setState(() {});
  }

  _statetChangeListener() {
    FlutterBluetoothSerial.instance
        .onStateChanged()
        .listen((BluetoothState state) {
      _bluetoothState = state;
      if (_bluetoothState.isEnabled) {
        _listBondedDevices();
      } else {
        myDevices.clear();
      }
      print("State isEnabled: ${state.isEnabled}");
      setState(() {});
    });
  }

  _listBondedDevices() {
    FlutterBluetoothSerial.instance
        .getBondedDevices()
        .then((List<BluetoothDevice> bondedDEvices) {
      myDevices = bondedDEvices;
    });
    setState(() {});
  }

  // void _startConnectionBT(BuildContext context, BluetoothDevice server) {
  //   Navigator.push(context, MaterialPageRoute(builder: (contex) {
  //     return MainPage(serverBT: server);
  //   }));
  // }

  void _startConnectionBT(BluetoothDevice myServer) {
    BluetoothConnection.toAddress(myServer.address).then((_connectionBT) {
      connectionBT = _connectionBT;
      isConnectedBT = connectionBT.isConnected;
      isDisconnectedBT = false;
      setState(() {});
      connectionBT.input.listen(_receiveByte).onDone(() {
        if (isDisconnectedBT) {
          print('Disconnecting locally');
        } else {
          print('Disconnecting remotely');
        }
        if (this.mounted) {
          setState(() {});
        }
        Navigator.of(context).pop();
      });
    }).catchError((error) {
      print(error);
    });
  }

  void _receiveByte(Uint8List data) {
    print(ascii.decode(data));
  }

  void _sendByte() async {
    try {
      print('Mengirim');
      connectionBT.output.add(ascii.encode('Mengirim data\n'));
      await connectionBT.output.allSent;
    } catch (e) {
      setState(() {});
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("Bluetooth Setting"),
      ),
      body: Container(
        child: Column(
          children: <Widget>[
            !isConnected
                ? ListTile(
                    title: Text("Bluetooth Status"),
                    subtitle: Text(_bluetoothState.toString()),
                    trailing: RaisedButton(
                      child: Text("Setting"),
                      onPressed: () {
                        FlutterBluetoothSerial.instance.openSettings();
                      },
                    ),
                  )
                : Container(width: 0, height: 0),
            Expanded(
              child: ListView(
                padding: EdgeInsets.only(bottom: 50),
                children: myDevices
                    .map((_devices) => BluetoothDeviceListEntry(
                          device: _devices,
                          enabled: true,
                          onTap: !isConnected
                              ? () {
                                  _startConnectionBT(_devices);
                                }
                              : null,
                        ))
                    .toList(),
              ),
            ),
            isConnectedBT
                ? Text("Device Connected !")
                : Text("Device Disconnected !"),
            FloatingActionButton(
              child: Icon(Icons.send),
              onPressed: isConnectedBT ? _sendByte : null,
              backgroundColor: Colors.greenAccent[700],
            )
          ],
        ),
      ),
    );
  }
}

class BluetoothDeviceListEntry extends ListTile {
  BluetoothDeviceListEntry({
    @required BluetoothDevice device,
    int rssi,
    GestureTapCallback onTap,
    GestureLongPressCallback onLongPress,
    bool enabled = true,
  }) : super(
          onTap: onTap,
          onLongPress: onLongPress,
          enabled: enabled,
          leading: Icon(Icons.devices),
          title: Text(device.name ?? "Unknown device"),
          subtitle: Text(rssi.toString()),
          trailing: Row(
            mainAxisSize: MainAxisSize.min,
            children: <Widget>[
              rssi != null
                  ? Container(
                      margin: new EdgeInsets.all(9.0),
                      child: DefaultTextStyle(
                        style: _computeTextStyle(rssi),
                        child: Column(
                          mainAxisSize: MainAxisSize.min,
                          children: <Widget>[
                            Text(rssi.toString()),
                            Text("dBm"),
                          ],
                        ),
                      ),
                    )
                  : Container(width: 0, height: 0),
              device.isConnected
                  ? Icon(Icons.import_export)
                  : Container(width: 0, height: 0),
              device.isBonded
                  ? Icon(Icons.link)
                  : Container(width: 0, height: 0),
            ],
          ),
        );

  static TextStyle _computeTextStyle(int rssi) {
    if (rssi >= -35)
      return TextStyle(color: Colors.green[700]);
    else if (rssi >= -45)
      return TextStyle(color: Colors.green[500]);
    else if (rssi >= -55)
      return TextStyle(color: Colors.blue[700]);
    else if (rssi >= -65)
      return TextStyle(color: Colors.blue[500]);
    else if (rssi >= -75)
      return TextStyle(color: Colors.lime[700]);
    else if (rssi >= -85)
      return TextStyle(color: Colors.amber[500]);
    else
      return TextStyle(color: Colors.redAccent[700]);
  }
}
