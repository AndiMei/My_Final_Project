// import 'dart:html';
import 'dart:io';
import 'dart:convert';
import 'dart:typed_data';
import 'dart:ui' as ui;

import 'package:flutter/material.dart';
import 'package:flutter_masked_text/flutter_masked_text.dart';
import 'package:web_socket_channel/io.dart';
import 'package:web_socket_channel/web_socket_channel.dart';
import 'package:web_socket_channel/status.dart' as status;

String _devicesIP = '192.168.001.001';
String sendByteMsg = "Test";
int charLength = _devicesIP.length;
String msgData = "";

class WifiPage extends StatefulWidget {
  @override
  _WifiPageState createState() => _WifiPageState();
}

class _WifiPageState extends State<WifiPage> with WidgetsBindingObserver {
  final textController =
      new MaskedTextController(mask: '000.000.000.000', text: '$_devicesIP');
  bool isConnectedWS = false;
  // var channel;
  WebSocketChannel channel;
  // WebSocket myWebSocket;

  void _sendByte() {
    channel.sink.add(sendByteMsg);
    setState(() {});
  }

  _onChangedtxt(String value) {
    setState(() {
      _devicesIP = value;
      charLength = value.length;
      // isConnectedWS =
      // charLength = 15;
    });
  }

  _onChangedTxtSend(String value) {
    setState(() {
      sendByteMsg = value;
    });
  }

  @override
  void dispose() {
    if (isConnectedWS) {
      channel.sink.add("CLOSE");
      channel.sink.close(status.goingAway);
    }
    print('kudune metu');
    super.dispose();
  }

  @override
  void initState() {
    // channel.sink.close();
    super.initState();
  }

  _startConnectionWS() async {
    try {
      print("mulai");
      channel = IOWebSocketChannel.connect("ws://$_devicesIP");
      channel.stream.listen((data) {
        msgData = data;
        if (msgData == "OK") {
          isConnectedWS = true;
          setState(() {});
        }
        if (isConnectedWS) {
          msgData = data;
        }
        print("DataReceived: " + data);
      });
    } catch (e) {
      print(e);
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("WiFi Setting"),
      ),
      body: Column(
        children: <Widget>[
          ListTile(
            title: Text("Bluetooth Status"),
            // subtitle: Text(_bluetoothState.toString()),
            trailing: RaisedButton(
              child: Text("Setting"),
              onPressed: () {
                // FlutterBluetoothSerial.instance.openSettings();
              },
            ),
          ),
          TextFormField(
            cursorColor: Theme.of(context).cursorColor,
            maxLength: 15,
            controller: textController,
            decoration: InputDecoration(
                icon: Icon(Icons.settings),
                labelText: 'IP Address',
                hintText: 'Enter IP Address Device',
                suffixIcon: charLength < 15
                    ? Icon(Icons.cancel, color: Colors.red)
                    : Icon(Icons.check_circle, color: Colors.green)),
            onChanged: _onChangedtxt,
          ),
          RaisedButton(
            child: Text(isConnectedWS ? 'Connected' : 'Connect'),
            color: isConnectedWS ? Colors.greenAccent[700] : Colors.grey[350],
            // onPressed: charLength < 15 ? null : _mulai,
            onPressed: charLength < 15 ? null : _startConnectionWS,
          ),
          FloatingActionButton(
            child: Icon(Icons.send),
            onPressed: isConnectedWS ? () {} : null,
            backgroundColor: Colors.greenAccent[700],
          )
          // isConnectedWS
          //     ? StreamBuilder(
          //         stream: channel.stream,
          //         builder: (context, snapshot) {
          //           if (snapshot.hasData) {}
          //           // return Padding(
          //           //     padding: EdgeInsets.symmetric(vertical: 24.0),
          //           //     child: Text('${snapshot.hasData}'));
          //           // snapshot.hasData ? '${snapshot.data}' : 'genek'));
          //         },
          //       )
          //     : Container(height: 0, width: 0)
        ],
      ),
    );
  }
}
