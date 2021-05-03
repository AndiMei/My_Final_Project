import 'dart:typed_data';
import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';

class MainPage extends StatefulWidget {
  final BluetoothDevice serverBT;
  const MainPage({this.serverBT});

  @override
  _MainPageState createState() => _MainPageState();
}

class _MainPageState extends State<MainPage> {
  BluetoothConnection connectionBT;
  bool isConnectedBT = true;

  bool get isConnected => connectionBT != null && connectionBT.isConnected;
  bool isDisconnectedBT = false;

  GlobalKey _globalKey = new GlobalKey();

  TextEditingController _textEditingController = new TextEditingController();

  @override
  void initState() {
    super.initState();
    _getBTConnection();
  }

  @override
  void dispose() {
    if (isConnected) {
      isConnectedBT = true;
      connectionBT.dispose();
      connectionBT = null;
    }
    super.dispose();
  }

  _getBTConnection() {
    BluetoothConnection.toAddress(widget.serverBT.address)
        .then((_connectionBT) {
      connectionBT = _connectionBT;
      isConnectedBT = false;
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
      Navigator.of(context).pop();
    });
  }

  void _receiveByte(Uint8List data) {
    print(ascii.decode(data));
  }

  void _sendByte() async {
    try {
      print('Mengirim');
      connectionBT.output.add(ascii.encode("BISMILLAH"));
      await connectionBT.output.allSent;
    } catch (e) {
      setState(() {});
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.blue[50],
      appBar: AppBar(
        title: (isConnectedBT
            ? Text('Connecting to ${widget.serverBT.name} ...')
            : isConnected
                ? Text('Connected with ${widget.serverBT.name}')
                : Text('Disconnected with ${widget.serverBT.name}')),
      ),
      body: SafeArea(
        child: isConnected
            ? Column(
                children: <Widget>[
                  Center(
                    child: sendButton(),
                  ),
                ],
              )
            : Center(
                child: Text(
                  'Connecting ...',
                  style: TextStyle(
                      fontSize: 24,
                      fontWeight: FontWeight.bold,
                      color: Colors.amber[400]),
                ),
              ),
      ),
    );
  }

  Widget sendButton() {
    return Container(
      padding: const EdgeInsets.all(16),
      child: RaisedButton(
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(10),
        ),
        onPressed: () {
          _sendByte();
        },
        color: Colors.blue[900],
        textColor: Colors.white,
        child: Padding(
          padding: const EdgeInsets.all(8.0),
          child: Text(
            'Send to ESP32',
            style: TextStyle(fontSize: 24),
          ),
        ),
      ),
    );
  }
}
