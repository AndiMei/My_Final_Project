import 'dart:ffi';
import 'dart:math';

import 'package:flutter/material.dart';
import 'package:flutter_knob/flutter_knob.dart';
import 'package:flutter_circular_slider/flutter_circular_slider.dart';
import 'package:parametric_eq_app/MyKnob.dart';
import 'bluetooth_page.dart';

// class MyKnob extends StatefulWidget {
//   final double value;
//   final double min;
//   final double max;

//   final ValueChanged<double> onChanged;
//   MyKnob({this.value, this.min = 0, this.max = 1, this.onChanged});

//   @override
//   State<StatefulWidget> createState() {
//     KnobState();
//   }
// }

class cobaPage extends StatefulWidget {
  @override
  _cobaPageState createState() => _cobaPageState();
}

class _cobaPageState extends State<cobaPage> {
  double nilai = 0;
  double nilai2 = 0;
  double nilai3 = 50;
  int nilai4 = 0;
  int laps = 0;
  int jumlah = 0;

  double _value = 50.0;

  int _preset = 0;
  int _band = 0;
  int _channel = 0;

  // Variable Left Channel
  double l_level = 0;

  double l_gain_1 = 0;
  double l_gain_2 = 0;
  double l_gain_3 = 0;
  double l_gain_4 = 0;
  double l_gain_5 = 0;

  double l_fc_1 = 50;
  double l_fc_2 = 50;
  double l_fc_3 = 50;
  double l_fc_4 = 50;
  double l_fc_5 = 50;

  double l_bw_1 = 0;
  double l_bw_2 = 0;
  double l_bw_3 = 0;

  // Variable Right Channel
  double r_level = 0;

  double r_gain_1 = 0;
  double r_gain_2 = 0;
  double r_gain_3 = 0;
  double r_gain_4 = 0;
  double r_gain_5 = 0;

  double r_fc_1 = 50;
  double r_fc_2 = 50;
  double r_fc_3 = 50;
  double r_fc_4 = 50;
  double r_fc_5 = 50;

  double r_bw_1 = 0;
  double r_bw_2 = 0;
  double r_bw_3 = 0;

  void _setValue(double value) => setState(() => _value = value);

  void _gain_l1(double newValue) {
    setState(() {
      l_gain_1 = newValue;
    });
    // sendToDevice();
  }

  double setStep(double value) {
    if (value >= 1000) {
      return 100;
    } else if (value >= 100) {
      return 10;
    } else {
      return 1;
    }
  }

  void sendToDevice(int preset, int band, int channel) {
    String strSend;
    String eqPreset;
    String eqBand;
    String eqChannel;
    String eqLevel;
    String eqGain;
    String eqFC;
    String eqBW;

    switch (band) {
      case 0:
        if (channel == 0) {
          eqGain = l_gain_1.toString() + '#';
          eqFC = l_fc_1.toString() + '\$';
        } else if (channel == 1) {
          eqGain = r_gain_1.toString() + '#';
          eqFC = r_fc_1.toString() + '\$';
        }
        break;

      case 1:
        if (channel == 0) {
          eqGain = l_gain_2.toString() + '#';
          eqFC = l_fc_2.toString() + '#';
          eqBW = l_bw_1.toString() + '\$';
        } else if (channel == 1) {
          eqGain = r_gain_2.toString() + '#';
          eqFC = r_fc_2.toString() + '#';
          eqBW = r_bw_1.toString() + '\$';
        }
        break;

      case 2:
        if (channel == 0) {
          eqGain = l_gain_3.toString() + '#';
          eqFC = l_fc_3.toString() + '#';
          eqBW = l_bw_2.toString() + '\$';
        } else if (channel == 1) {
          eqGain = r_gain_3.toString() + '#';
          eqFC = r_fc_3.toString() + '#';
          eqBW = r_bw_2.toString() + '\$';
        }
        break;

      case 3:
        if (channel == 0) {
          eqGain = l_gain_4.toString() + '#';
          eqFC = l_fc_4.toString() + '#';
          eqBW = l_bw_3.toString() + '\$';
        } else if (channel == 1) {
          eqGain = r_gain_4.toString() + '#';
          eqFC = r_fc_4.toString() + '#';
          eqBW = r_bw_3.toString() + '\$';
        }
        break;

      case 4:
        if (channel == 0) {
          eqGain = l_gain_5.toString() + '#';
          eqFC = l_fc_5.toString() + '\$';
        } else if (channel == 1) {
          eqGain = r_gain_5.toString() + '#';
          eqFC = r_fc_5.toString() + '\$';
        }
        break;
    }

    eqPreset = preset.toString() + '#';
    eqBand = band.toString() + '#';
    eqChannel = channel.toString() + '#';

    if (channel == 0) {
      eqLevel = l_level.toString() + '#';
    } else if (channel == 1) {
      eqLevel = r_level.toString() + '#';
    }

    if ((band > 0) && (band < 4)) {
      strSend = eqPreset + eqBand + eqChannel + eqLevel + eqGain + eqFC + eqBW;
    } else {
      strSend = eqPreset + eqBand + eqChannel + eqLevel + eqGain + eqFC;
    }

    // connectionBT.output.add(ascii.encode('BISMILLAH\$\n'));
    print(strSend);
  }

  static const double minValue = 50;
  static const double maxValue = 18000;

  // static const double minAngle = -160;
  // static const double maxAngle = 160;
  // static const double sweepAngle = maxAngle - minAngle;
  // static const double distanceToAngle = 0.007 * (maxValue - minValue);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("EQ-JOSS"),
        // leading: GestureDetector(
        //   onTap: () {
        //     Scaffold.of(context).openDrawer();
        //   },
        //   child: Icon(Icons.menu),
        // ),
      ),
      body: menuEQ(context),
      drawer: MyDrawer(),
      //   body: Stack(
      // children: <Widget>[
      //   menuEQ(context),
      //   // Text("Ngeseng"),
      // ],
    );
    // appBar: AppBar(
    //   title: Text("EQ-JOSS"),
    //   // leading: GestureDetector(
    //   //   onTap: null,
    //   //   child: Icon(Icons.menu),
    //   // ),
    //   actions: <Widget>[
    //     PopupMenuButton<String>(
    //       onSelected: null,
    //       itemBuilder: (BuildContext context) {
    //         return {'Logout', 'Settings'}.map((String choice) {
    //           return PopupMenuItem<String>(
    //             value: choice,
    //             child: Text(choice),
    //           );
    //         }).toList();
    //       },
    //     ),
    //     // Padding(
    //     //   padding: EdgeInsets.only(right: 20.0),
    //     //   child: GestureDetector(
    //     //       onTap: null,
    //     //       child: Icon(
    //     //         Icons.menu,
    //     //         size: 26.0,
    //     //       )),
    //     // )
    //   ],
    // ),
    // body: menuEQ(context),
    // menuEQ(context);
    // );
  }

  // Widget drawer() {
  //   return Column(
  //     children: <Widget>[
  //       Text("TEST"),
  //     ],
  //   );
  // }

  Widget menuEQ(context) {
    return Row(
      mainAxisAlignment: MainAxisAlignment.center,
      children: <Widget>[
        Flexible(
          flex: 1,
          child: Column(
            children: <Widget>[
              // L Gain ~ LOW
              Flexible(
                flex: 2,
                child: Container(
                  color: Color.fromARGB(255, 56, 56, 99),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Container(
                        height: 180,
                        color: Color.fromARGB(255, 56, 56, 99),
                        child: RotatedBox(
                          quarterTurns: 3,
                          child: SliderTheme(
                            data: SliderThemeData(
                              trackHeight: 10,
                              thumbShape:
                                  RoundSliderThumbShape(enabledThumbRadius: 16),
                              activeTrackColor: Colors.blue[800],
                              inactiveTrackColor: Colors.red[900],
                              thumbColor: Colors.black,
                            ),
                            child: Slider(
                              value: l_gain_1,
                              min: -24,
                              max: 24,
                              divisions: 96,
                              // onChanged: _gain_l1,
                              onChanged: (l_gain_1) => setState(() {
                                this.l_gain_1 = l_gain_1;
                                _band = 0;
                                sendToDevice(_preset, _band, 0);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$l_gain_1',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              // L FC ~ LOW
              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 36, 36, 71),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: l_fc_1,
                          min: 50,
                          max: 16000,
                          step: setStep(l_fc_1),
                          onChanged: (l_fc_1) => setState(() {
                            this.l_fc_1 = l_fc_1;
                            _band = 0;
                            sendToDevice(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$l_fc_1',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),
              Flexible(
                flex: 1,
                child: Container(
                  color: Color.fromARGB(255, 21, 21, 46),
                ),
              ),
            ],
          ),
        ),
        Flexible(
          flex: 1,
          child: Column(
            children: <Widget>[
              // L Gain ~ LOW-MID
              Flexible(
                flex: 2,
                child: Container(
                  color: Color.fromARGB(255, 56, 56, 99),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Container(
                        height: 180,
                        color: Color.fromARGB(255, 56, 56, 99),
                        child: RotatedBox(
                          quarterTurns: 3,
                          child: SliderTheme(
                            data: SliderThemeData(
                              trackHeight: 10,
                              thumbShape:
                                  RoundSliderThumbShape(enabledThumbRadius: 16),
                              activeTrackColor: Colors.blue[800],
                              inactiveTrackColor: Colors.red[900],
                              thumbColor: Colors.black,
                            ),
                            child: Slider(
                              value: l_gain_2,
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (l_gain_2) => setState(() {
                                this.l_gain_2 = l_gain_2;
                                _band = 1;
                                sendToDevice(_preset, _band, 0);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$l_gain_2',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              // L FC ~ LOW-MID
              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 36, 36, 71),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: l_fc_2,
                          min: 50,
                          max: 16000,
                          step: setStep(l_fc_2),
                          onChanged: (l_fc_2) => setState(() {
                            this.l_fc_2 = l_fc_2;
                            _band = 1;
                            sendToDevice(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$l_fc_2',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),
              // L BW ~ LOW-MID
              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 21, 21, 46),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: l_bw_1,
                          min: 0,
                          max: 100,
                          step: setStep(l_bw_1),
                          onChanged: (l_bw_1) => setState(() {
                            this.l_bw_1 = l_bw_1;
                            _band = 1;
                            sendToDevice(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$l_bw_1',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
        Flexible(
          flex: 1,
          child: Column(
            children: <Widget>[
              // L Gain ~ MID
              Flexible(
                flex: 2,
                child: Container(
                  color: Color.fromARGB(255, 56, 56, 99),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Container(
                        height: 180,
                        color: Color.fromARGB(255, 56, 56, 99),
                        child: RotatedBox(
                          quarterTurns: 3,
                          child: SliderTheme(
                            data: SliderThemeData(
                              trackHeight: 10,
                              thumbShape:
                                  RoundSliderThumbShape(enabledThumbRadius: 16),
                              activeTrackColor: Colors.blue[800],
                              inactiveTrackColor: Colors.red[900],
                              thumbColor: Colors.black,
                            ),
                            child: Slider(
                              value: l_gain_3,
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (l_gain_3) => setState(() {
                                this.l_gain_3 = l_gain_3;
                                _band = 2;
                                sendToDevice(_preset, _band, 0);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$l_gain_3',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              // L FC ~ MID
              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 36, 36, 71),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: l_fc_3,
                          min: 50,
                          max: 16000,
                          step: setStep(l_fc_3),
                          onChanged: (l_fc_3) => setState(() {
                            this.l_fc_3 = l_fc_3;
                            _band = 2;
                            sendToDevice(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$l_fc_3',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),

              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 21, 21, 46),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: l_bw_2,
                          min: 0,
                          max: 100,
                          step: setStep(l_bw_2),
                          onChanged: (l_bw_2) => setState(() {
                            this.l_bw_2 = l_bw_2;
                            _band = 2;
                            sendToDevice(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$l_bw_2',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
        Flexible(
          flex: 1,
          child: Column(
            children: <Widget>[
              // L Gain ~ MID-HIGH
              Flexible(
                flex: 2,
                child: Container(
                  color: Color.fromARGB(255, 56, 56, 99),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Container(
                        height: 180,
                        color: Color.fromARGB(255, 56, 56, 99),
                        child: RotatedBox(
                          quarterTurns: 3,
                          child: SliderTheme(
                            data: SliderThemeData(
                              trackHeight: 10,
                              thumbShape:
                                  RoundSliderThumbShape(enabledThumbRadius: 16),
                              activeTrackColor: Colors.blue[800],
                              inactiveTrackColor: Colors.red[900],
                              thumbColor: Colors.black,
                            ),
                            child: Slider(
                              value: l_gain_4,
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (l_gain_4) => setState(() {
                                this.l_gain_4 = l_gain_4;
                                _band = 3;
                                sendToDevice(_preset, _band, 0);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$l_gain_4',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              // L FC ~ MID-HIGH
              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 36, 36, 71),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: l_fc_4,
                          min: 50,
                          max: 16000,
                          step: setStep(l_fc_4),
                          onChanged: (l_fc_4) => setState(() {
                            this.l_fc_4 = l_fc_4;
                            _band = 3;
                            sendToDevice(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$l_fc_4',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),
              // L BW ~ MID-HIGH
              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 21, 21, 46),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: l_bw_3,
                          min: 0,
                          max: 100,
                          step: setStep(l_bw_3),
                          onChanged: (l_bw_3) => setState(() {
                            this.l_bw_3 = l_bw_3;
                            _band = 3;
                            sendToDevice(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$l_bw_3',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
        // L Gain ~ HIGH
        Flexible(
          flex: 1,
          child: Column(
            children: <Widget>[
              // L Band LOW
              Flexible(
                flex: 2,
                child: Container(
                  color: Color.fromARGB(255, 56, 56, 99),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Container(
                        height: 180,
                        color: Color.fromARGB(255, 56, 56, 99),
                        child: RotatedBox(
                          quarterTurns: 3,
                          child: SliderTheme(
                            data: SliderThemeData(
                              trackHeight: 10,
                              thumbShape:
                                  RoundSliderThumbShape(enabledThumbRadius: 16),
                              activeTrackColor: Colors.blue[800],
                              inactiveTrackColor: Colors.red[900],
                              thumbColor: Colors.black,
                            ),
                            child: Slider(
                              value: l_gain_5,
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (l_gain_5) => setState(() {
                                this.l_gain_5 = l_gain_5;
                                _band = 4;
                                sendToDevice(_preset, _band, 0);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$l_gain_5',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              // L FC ~ HIGH
              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 36, 36, 71),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: l_fc_5,
                          min: 50,
                          max: 16000,
                          step: setStep(l_fc_5),
                          onChanged: (l_fc_5) => setState(() {
                            this.l_fc_5 = l_fc_5;
                            _band = 4;
                            sendToDevice(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$l_fc_5',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),
              Flexible(
                flex: 1,
                child: Container(
                  color: Color.fromARGB(255, 21, 21, 46),
                ),
              ),
            ],
          ),
        ),
        // ============================= L Level ============================
        Flexible(
            flex: 1,
            child: Container(
              padding: new EdgeInsets.symmetric(vertical: 30.0),
              color: Color.fromARGB(255, 119, 95, 176),
              child: Column(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: <Widget>[
                  Container(
                    height: 300,
                    child: RotatedBox(
                      quarterTurns: 3,
                      child: SliderTheme(
                        data: SliderThemeData(
                          trackHeight: 10,
                          thumbShape:
                              RoundSliderThumbShape(enabledThumbRadius: 16),
                          activeTrackColor: Colors.blue[800],
                          inactiveTrackColor: Colors.red[900],
                          thumbColor: Colors.black,
                        ),
                        child: Slider(
                          value: l_level,
                          min: 0,
                          max: 100,
                          divisions: 50,
                          onChanged: (l_level) => setState(() {
                            this.l_level = l_level;
                            sendToDevice(_preset, _band, 0);
                          }),
                        ),
                      ),
                    ),
                  ),
                  Align(
                    alignment: Alignment.bottomCenter,
                    child: Text(
                      '${l_level.round()}',
                      style: TextStyle(
                          color: Colors.white,
                          fontSize: 17,
                          fontStyle: FontStyle.italic),
                    ),
                  ),
                ],
              ),
            )),
        //============================= R Level ============================
        Flexible(
            flex: 1,
            child: Container(
              padding: new EdgeInsets.symmetric(vertical: 30.0),
              color: Color.fromARGB(255, 119, 95, 176),
              child: Column(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: <Widget>[
                  Container(
                    height: 300,
                    child: RotatedBox(
                      quarterTurns: 3,
                      child: SliderTheme(
                        data: SliderThemeData(
                          trackHeight: 10,
                          thumbShape:
                              RoundSliderThumbShape(enabledThumbRadius: 16),
                          activeTrackColor: Colors.blue[800],
                          inactiveTrackColor: Colors.red[900],
                          thumbColor: Colors.black,
                        ),
                        child: Slider(
                          value: r_level,
                          min: 0,
                          max: 100,
                          divisions: 50,
                          onChanged: (r_level) => setState(() {
                            this.r_level = r_level;
                            sendToDevice(_preset, _band, 1);
                          }),
                        ),
                      ),
                    ),
                  ),
                  Align(
                    alignment: Alignment.bottomCenter,
                    child: Text(
                      '${r_level.round()}',
                      style: TextStyle(
                          color: Colors.white,
                          fontSize: 17,
                          fontStyle: FontStyle.italic),
                    ),
                  ),
                ],
              ),
            )),
        Flexible(
          flex: 1,
          child: Column(
            children: <Widget>[
              // L Gain ~ LOW
              Flexible(
                flex: 2,
                child: Container(
                  color: Color.fromARGB(255, 56, 56, 99),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Container(
                        height: 180,
                        color: Color.fromARGB(255, 56, 56, 99),
                        child: RotatedBox(
                          quarterTurns: 3,
                          child: SliderTheme(
                            data: SliderThemeData(
                              trackHeight: 10,
                              thumbShape:
                                  RoundSliderThumbShape(enabledThumbRadius: 16),
                              activeTrackColor: Colors.blue[800],
                              inactiveTrackColor: Colors.red[900],
                              thumbColor: Colors.black,
                            ),
                            child: Slider(
                              value: r_gain_1,
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (r_gain_1) => setState(() {
                                this.r_gain_1 = r_gain_1;
                                _band = 0;
                                sendToDevice(_preset, _band, 1);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$r_gain_1',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              // R FC ~ LOW
              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 36, 36, 71),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: r_fc_1,
                          min: 50,
                          max: 16000,
                          step: setStep(r_fc_1),
                          onChanged: (r_fc_1) => setState(() {
                            this.r_fc_1 = r_fc_1;
                            _band = 0;
                            sendToDevice(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$r_fc_1',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),
              Flexible(
                flex: 1,
                child: Container(
                  color: Color.fromARGB(255, 21, 21, 46),
                ),
              ),
            ],
          ),
        ),
        Flexible(
          flex: 1,
          child: Column(
            children: <Widget>[
              // R Gain ~ LOW-MID
              Flexible(
                flex: 2,
                child: Container(
                  color: Color.fromARGB(255, 56, 56, 99),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Container(
                        height: 180,
                        color: Color.fromARGB(255, 56, 56, 99),
                        child: RotatedBox(
                          quarterTurns: 3,
                          child: SliderTheme(
                            data: SliderThemeData(
                              trackHeight: 10,
                              thumbShape:
                                  RoundSliderThumbShape(enabledThumbRadius: 16),
                              activeTrackColor: Colors.blue[800],
                              inactiveTrackColor: Colors.red[900],
                              thumbColor: Colors.black,
                            ),
                            child: Slider(
                              value: r_gain_2,
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (r_gain_2) => setState(() {
                                this.r_gain_2 = r_gain_2;
                                _band = 1;
                                sendToDevice(_preset, _band, 1);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$r_gain_2',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              // R FC ~ LOW-MID
              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 36, 36, 71),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: r_fc_2,
                          min: 50,
                          max: 16000,
                          step: setStep(r_fc_2),
                          onChanged: (r_fc_2) => setState(() {
                            this.r_fc_2 = r_fc_2;
                            _band = 1;
                            sendToDevice(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$r_fc_2',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),
              // R BW ~ LOW-MID
              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 21, 21, 46),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: r_bw_1,
                          min: 0,
                          max: 100,
                          step: setStep(r_bw_1),
                          onChanged: (r_bw_1) => setState(() {
                            this.r_bw_1 = r_bw_1;
                            _band = 1;
                            sendToDevice(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$r_bw_1',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
        // BAND 2 (MID)
        Flexible(
          flex: 1,
          child: Column(
            children: <Widget>[
              // L GAIN ~ MID
              Flexible(
                flex: 2,
                child: Container(
                  color: Color.fromARGB(255, 56, 56, 99),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Container(
                        height: 180,
                        color: Color.fromARGB(255, 56, 56, 99),
                        child: RotatedBox(
                          quarterTurns: 3,
                          child: SliderTheme(
                            data: SliderThemeData(
                              trackHeight: 10,
                              thumbShape:
                                  RoundSliderThumbShape(enabledThumbRadius: 16),
                              activeTrackColor: Colors.blue[800],
                              inactiveTrackColor: Colors.red[900],
                              thumbColor: Colors.black,
                            ),
                            child: Slider(
                              value: r_gain_3,
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (r_gain_3) => setState(() {
                                this.r_gain_3 = r_gain_3;
                                _band = 2;
                                sendToDevice(_preset, _band, 1);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$r_gain_3',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 36, 36, 71),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: r_fc_3,
                          min: 50,
                          max: 16000,
                          step: setStep(r_fc_3),
                          onChanged: (r_fc_3) => setState(() {
                            this.r_fc_3 = r_fc_3;
                            _band = 2;
                            sendToDevice(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$r_fc_3',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),

              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 21, 21, 46),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: r_bw_2,
                          min: 0,
                          max: 100,
                          step: setStep(r_bw_2),
                          onChanged: (r_bw_2) => setState(() {
                            this.r_bw_2 = r_bw_2;
                            _band = 2;
                            sendToDevice(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$r_bw_2',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
        Flexible(
          flex: 1,
          child: Column(
            children: <Widget>[
              // L Band LOW-MID
              Flexible(
                flex: 2,
                child: Container(
                  color: Color.fromARGB(255, 56, 56, 99),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Container(
                        height: 180,
                        color: Color.fromARGB(255, 56, 56, 99),
                        child: RotatedBox(
                          quarterTurns: 3,
                          child: SliderTheme(
                            data: SliderThemeData(
                              trackHeight: 10,
                              thumbShape:
                                  RoundSliderThumbShape(enabledThumbRadius: 16),
                              activeTrackColor: Colors.blue[800],
                              inactiveTrackColor: Colors.red[900],
                              thumbColor: Colors.black,
                            ),
                            child: Slider(
                              value: r_gain_4,
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (r_gain_4) => setState(() {
                                this.r_gain_4 = r_gain_4;
                                _band = 3;
                                sendToDevice(_preset, _band, 1);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$r_gain_4',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 36, 36, 71),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: r_fc_4,
                          min: 50,
                          max: 16000,
                          step: setStep(r_fc_4),
                          onChanged: (r_fc_4) => setState(() {
                            this.r_fc_4 = r_fc_4;
                            _band = 3;
                            sendToDevice(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$r_fc_4',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),

              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 21, 21, 46),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: r_bw_3,
                          min: 0,
                          max: 100,
                          step: setStep(r_bw_3),
                          onChanged: (r_bw_3) => setState(() {
                            this.r_bw_3 = r_bw_3;
                            _band = 3;
                            sendToDevice(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$r_bw_3',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
        Flexible(
          flex: 1,
          child: Column(
            children: <Widget>[
              // L Band LOW
              Flexible(
                flex: 2,
                child: Container(
                  color: Color.fromARGB(255, 56, 56, 99),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Container(
                        height: 180,
                        color: Color.fromARGB(255, 56, 56, 99),
                        child: RotatedBox(
                          quarterTurns: 3,
                          child: SliderTheme(
                            data: SliderThemeData(
                              trackHeight: 10,
                              thumbShape:
                                  RoundSliderThumbShape(enabledThumbRadius: 16),
                              activeTrackColor: Colors.blue[800],
                              inactiveTrackColor: Colors.red[900],
                              thumbColor: Colors.black,
                            ),
                            child: Slider(
                              value: r_gain_5,
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (r_gain_5) => setState(() {
                                this.r_gain_5 = r_gain_5;
                                _band = 4;
                                sendToDevice(_preset, _band, 1);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$r_gain_5',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              Flexible(
                child: Container(
                  color: Color.fromARGB(255, 36, 36, 71),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      Center(
                        child: MyKnob(
                          value: r_fc_5,
                          min: 50,
                          max: 16000,
                          step: setStep(r_fc_5),
                          onChanged: (r_fc_5) => setState(() {
                            this.r_fc_5 = r_fc_5;
                            _band = 4;
                            sendToDevice(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          '$r_fc_5',
                          style: TextStyle(
                              color: Colors.white,
                              fontSize: 17,
                              fontStyle: FontStyle.italic),
                        ),
                      )
                    ],
                  ),
                ),
              ),
              Flexible(
                flex: 1,
                child: Container(
                  color: Color.fromARGB(255, 21, 21, 46),
                ),
              ),
            ],
          ),
        ),
      ],
    );
  }
}

class MyDrawer extends StatelessWidget {
  final Function onTap;

  MyDrawer({this.onTap});
  @override
  Widget build(BuildContext context) {
    return SizedBox(
        width: MediaQuery.of(context).size.width * 0.2,
        child: Drawer(
            child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: <Widget>[
            DrawerHeader(
              decoration: BoxDecoration(color: Colors.blue),
              child: Padding(
                padding: EdgeInsets.all(6),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  mainAxisAlignment: MainAxisAlignment.end,
                  children: <Widget>[
                    Text("BISMILLAH"),
                  ],
                ),
              ),
            ),
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceAround,
              children: <Widget>[
                FloatingActionButton(
                  child: Icon(IconData(0xe15b, fontFamily: 'MaterialIcons')),
                  onPressed: () {},
                ),
                Text('1'),
                FloatingActionButton(
                  child: Icon(
                    Icons.add,
                    color: Colors.black,
                  ),
                  onPressed: () {},
                ),
              ],
            ),
            ListTile(
              leading: Icon(Icons.copy),
              title: Text("Copy Preset"),
              onTap: () {},
            )
          ],
        )));
  }
}

class CustomTrackShape extends RoundedRectSliderTrackShape {
  Rect getPreferredRect({
    @required RenderBox parentBox,
    Offset offset = Offset.zero,
    @required SliderThemeData sliderTheme,
    bool isEnabled = false,
    bool isDiscrete = false,
  }) {
    final double trackHeight = sliderTheme.trackHeight;
    final double trackLeft = offset.dx;
    final double trackTop =
        offset.dy + (parentBox.size.height - trackHeight) / 2;
    final double trackWidth = parentBox.size.width;
    return Rect.fromLTWH(trackLeft, trackTop, trackWidth * 2, trackHeight);
    // return Rect.fromLTWH(0, 0, 2, 2);
  }
}

class CustomTrackShape2 extends RoundedRectSliderTrackShape {
  Rect getPreferredRect({
    @required RenderBox parentBox,
    Offset offset = Offset.zero,
    @required SliderThemeData sliderTheme,
    bool isEnabled = false,
    bool isDiscrete = false,
  }) {
    final double trackHeight = sliderTheme.trackHeight;
    final double trackLeft = offset.dx - 20;
    final double trackTop =
        offset.dy + (parentBox.size.height - trackHeight) / 2;
    final double trackWidth = parentBox.size.width;
    // print('left $trackWidth');
    return Rect.fromLTWH(trackLeft, trackTop, trackWidth + 40, trackHeight);
  }
}
