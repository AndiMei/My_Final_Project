import 'package:flutter/material.dart';

class cobaPage extends StatefulWidget {
  @override
  _cobaPageState createState() => _cobaPageState();
}

class MyVector {
  final x;
  final y;
  const MyVector(this.y, this.x);
}

class _cobaPageState extends State<cobaPage> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text("ini coba lo")),
      // body: widget<>,
    );
  }
}
