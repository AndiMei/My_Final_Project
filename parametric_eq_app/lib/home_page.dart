import 'package:flutter/material.dart';
import 'bluetooth_page.dart';
import 'wifi_page.dart';

class HomePage extends StatefulWidget {
  @override
  _HomePageState createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("EQ-JOSS V1.0"),
      ),
      body: Column(
        children: <Widget>[
          RaisedButton(
            child: Text("Bluetooth"),
            onPressed: () {
              Navigator.push(context, MaterialPageRoute(builder: (contex) {
                return BluetoothPage();
              }));
            },
          ),
          RaisedButton(
            child: Text("WiFi"),
            onPressed: () {
              Navigator.push(context, MaterialPageRoute(builder: (contex) {
                return WifiPage();
              }));
            },
          ),
        ],
      ),
    );
  }
}
