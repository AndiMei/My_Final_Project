import 'package:flutter/material.dart';
import 'bluetooth_page.dart';
import 'wifi_page.dart';
import 'coba.dart';

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
        // crossAxisAlignment: CrossAxisAlignment.end,
        // mainAxisAlignment: MainAxisAlignment.spaceAround,
        children: <Widget>[
          Flexible(
            flex: 1,
            child: Container(
              height: MediaQuery.of(context).size.height,
              width: MediaQuery.of(context).size.width,
              child: RaisedButton(
                color: Colors.purple[800],
                child: Text(
                  'Bluetooth',
                  style: TextStyle(
                    fontWeight: FontWeight.bold,
                    fontSize: 30,
                    color: Colors.white,
                  ),
                ),
                onPressed: () {
                  Navigator.push(context, MaterialPageRoute(builder: (contex) {
                    return BluetoothPage();
                  }));
                },
              ),
            ),
          ),
          Flexible(
            flex: 1,
            child: Container(
              height: MediaQuery.of(context).size.height,
              width: MediaQuery.of(context).size.width,
              child: RaisedButton(
                color: Colors.pink[900],
                child: Text(
                  'WiFi',
                  style: TextStyle(
                    fontWeight: FontWeight.bold,
                    fontSize: 30,
                    color: Colors.white,
                  ),
                ),
                onPressed: () {
                  Navigator.push(context, MaterialPageRoute(builder: (contex) {
                    return WifiPage();
                  }));
                },
              ),
            ),
          ),
          Flexible(
            flex: 1,
            child: Container(
              height: MediaQuery.of(context).size.height,
              width: MediaQuery.of(context).size.width,
              color: Colors.purple,
              child: RaisedButton(
                child: Text("Test EQ"),
                onPressed: () {
                  Navigator.push(context, MaterialPageRoute(builder: (contex) {
                    return cobaPage();
                  }));
                },
              ),
            ),
          ),
        ],
      ),
    );
  }
}
