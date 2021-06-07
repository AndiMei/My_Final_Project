import 'package:flutter/material.dart';
import 'bluetooth_page.dart';
import 'wifi_page.dart';
import 'eq_page.dart';

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
      body: Center(
        child: Column(
          // crossAxisAlignment: CrossAxisAlignment.center,
          // mainAxisAlignment: MainAxisAlignment.spaceAround,
          // mainAxisSize: MainAxisSize.min,
          children: <Widget>[
            Container(
              padding: EdgeInsets.only(top: 50, bottom: 20),
              child: Text(
                "Select Connection",
                style: TextStyle(
                  color: Colors.purple[900],
                  fontWeight: FontWeight.bold,
                  fontStyle: FontStyle.italic,
                ),
              ),
            ),
            RaisedButton(
              color: Colors.purple[900],
              shape: StadiumBorder(),
              child: Text(
                "Bluetooth",
                style: TextStyle(
                  color: Colors.white,
                  fontWeight: FontWeight.bold,
                  fontStyle: FontStyle.italic,
                ),
              ),
              onPressed: () {
                Navigator.push(context, MaterialPageRoute(builder: (contex) {
                  return BluetoothPage();
                }));
              },
            ),
            RaisedButton(
              color: Colors.purple[900],
              shape: StadiumBorder(),
              child: Text(
                "WiFi",
                style: TextStyle(
                  color: Colors.white,
                  fontWeight: FontWeight.bold,
                  fontStyle: FontStyle.italic,
                ),
              ),
              onPressed: () {
                Navigator.push(context, MaterialPageRoute(builder: (contex) {
                  return WifiPage();
                }));
              },
            ),
            RaisedButton(
              color: Colors.red,
              shape: StadiumBorder(),
              child: Text(
                "Debug EQ",
                style: TextStyle(
                  color: Colors.white,
                  fontWeight: FontWeight.bold,
                  fontStyle: FontStyle.italic,
                ),
              ),
              onPressed: () {
                Navigator.push(context, MaterialPageRoute(builder: (contex) {
                  return eqPage();
                }));
              },
            ),
          ],
        ),
      ),
    );
  }
}
