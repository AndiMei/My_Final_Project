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

import 'eq_page.dart';
import 'streamer/stream_data.dart';

String _devicesIP = '192.168.001.001';
String sendByteMsg = "RP0\n";
int charLength = _devicesIP.length;
String msgData = "";

bool pisan;

class WifiPage extends StatefulWidget {
  @override
  _WifiPageState createState() => _WifiPageState();
}

class _WifiPageState extends State<WifiPage> with WidgetsBindingObserver {
  WebSocketChannel channel;

  bool isConnectedWS = false;
  final textController =
      new MaskedTextController(mask: '000.000.000.000', text: '$_devicesIP');

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
    print("Init");

    Stream stream = streamTransmit.stream;
    stream.listen((dataTx) {
      channel.sink.add(dataTx);
      setState(() {});
    });
    super.initState();
  }

  // WebSocket myWebSocket;

  void _gotoEQ() {
    Navigator.push(context, MaterialPageRoute(builder: (contex) {
      return eqPage();
    }));
    setState(() {});
  }

  _onChangedtxt(String value) {
    setState(() {
      _devicesIP = value;
      charLength = value.length;
    });
  }

  _onChangedTxtSend(String value) {
    setState(() {
      sendByteMsg = value;
    });
  }

  _startConnectionWS() async {
    try {
      print("mulai");
      channel = IOWebSocketChannel.connect("ws://$_devicesIP");
      channel.stream.listen((dataRx) {
        msgData = dataRx;
        if (msgData == "OK") {
          isConnectedWS = true;
          setState(() {});
        }
        if (isConnectedWS) {
          streamReceive.add(dataRx);
        }
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
        actions: <Widget>[
          isConnectedWS
              ? Icon(
                  Icons.wifi,
                  size: 40,
                  color: Colors.green,
                )
              : Icon(
                  Icons.perm_scan_wifi_outlined,
                  size: 40,
                  color: Colors.grey,
                )
        ],
      ),
      body: Column(
        children: <Widget>[
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
            color: isConnectedWS ? Colors.greenAccent[400] : Colors.grey[350],
            onPressed: charLength < 15 ? null : _startConnectionWS,
          ),
          RaisedButton(
            child: Text("Goto EQ"),
            onPressed: isConnectedWS ? _gotoEQ : null,
            color: isConnectedWS ? Colors.greenAccent[400] : Colors.grey,
          ),
        ],
      ),
    );
  }
}
