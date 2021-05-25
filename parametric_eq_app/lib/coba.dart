import 'dart:convert';
import 'dart:ffi';
import 'dart:math';

import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:flutter_knob/flutter_knob.dart';
import 'package:flutter_circular_slider/flutter_circular_slider.dart';
import 'package:parametric_eq_app/MyKnob.dart';
import 'bluetooth_page.dart';

import 'streamer/stream_data.dart';

const int MAX_PRESET = 10;
const int MAX_BAND = 5;

class cobaPage extends StatefulWidget {
  @override
  _cobaPageState createState() => _cobaPageState();
}

class _cobaPageState extends State<cobaPage> {
  int _preset = 0;
  int _band = 0;
  int _channel = 0;

  var l_gain = new List(MAX_BAND);
  var l_fc = new List(MAX_BAND);
  var l_bw = new List(MAX_BAND - 2);

  var r_gain = new List(MAX_BAND);
  var r_fc = new List(MAX_BAND);
  var r_bw = new List(MAX_BAND - 2);

  _cobaPageState() {
    for (int i = 0; i < MAX_BAND; i++) {
      l_gain[i] = 0.0;
      l_fc[i] = 50.0;

      r_gain[i] = 0.0;
      r_fc[i] = 50.0;

      if (i < 3) {
        l_bw[i] = 0.0;
        r_bw[i] = 0.0;
      }
    }
  }

  @override
  void initState() {
    super.initState();
    requestPreset(0);

    Stream stream = streamReceive.stream;
    stream.listen((data) {
      print(data);
      readPreset(data);
    });
  }

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

  double setStep(double value) {
    if (value >= 1000) {
      return 100;
    } else if (value >= 100) {
      return 10;
    } else {
      return 1;
    }
  }

  bool enWritePreset;
  void readPreset(String cmd) {
    if (cmd == 'MWP\n') {
      enWritePreset = true;
      // print("masuk sini");
    }

    if (enWritePreset == true) {
      if (cmd != 'MWP\n') {
        // print("cmd" + cmd);
        enWritePreset = false;
        var arr = cmd.split('#');

        _band = int.parse(arr[1]);
        _channel = int.parse(arr[2]);

        if (_channel == 0) {
          l_level = double.parse(arr[3]);
          l_gain[_band] = double.parse(arr[4]);
          l_fc[_band] = double.parse(arr[5]);
          if ((_band > 0) && (_band < 4)) {
            l_bw[_band - 1] = double.parse(arr[6]);
          }
        } else if (_channel == 1) {
          r_level = double.parse(arr[3]);
          r_gain[_band] = double.parse(arr[4]);
          r_fc[_band] = double.parse(arr[5]);
          if ((_band > 0) && (_band < 4)) {
            r_bw[_band - 1] = double.parse(arr[6]);
          }
        }
        // print("ini band: $_band");
      }

      setState(() {});
    }
  }

  void writePreset(int preset, int band, int channel) {
    String strSend;
    String eqPreset;
    String eqBand;
    String eqChannel;
    String eqLevel;
    String eqGain;
    String eqFC;
    String eqBW;

    if (channel == 0) {
      eqGain = l_gain[band].toString() + '#';
      if ((band > 0) && (band < 4)) {
        eqFC = l_fc[band].toString() + '#';
        eqBW = l_bw[band - 1].toString() + '\n';
      } else {
        eqFC = l_fc[band].toString() + '\n';
      }
    } else if (channel == 1) {
      eqGain = r_gain[band].toString() + '#';
      if ((band > 0) && (band < 4)) {
        eqFC = r_fc[band].toString() + '#';
        eqBW = r_bw[band - 1].toString() + '\n';
      } else {
        eqFC = r_fc[band].toString() + '\n';
      }
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

    print('WP\n' + strSend);
    // sendDataBloc.feedSendData('WP\n' + strSend);
    streamTransmit.add('WP\n' + strSend);
  }

  void requestPreset(int preset) {
    String strSend;

    strSend = 'RP$preset\n';
    print(strSend);
    // sendDataBloc.feedSendData(strSend);
    streamTransmit.add(strSend);
    // _streamData();
  }

  // void _receiveBytes(String data) {
  //   print(data);
  // }

  // void _streamData() {
  //   returnDataBloc.streamReturnData.listen(_receiveBytes).onDone(() {});
  // }

  static const double minValue = 50;
  static const double maxValue = 18000;

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
      drawer: SizedBox(
          width: MediaQuery.of(context).size.width * 0.2,
          child: Drawer(
              child: Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              // DrawerHeader(
              //   decoration: BoxDecoration(color: Colors.green),
              //   // child: Padding(
              //   // padding: EdgeInsets.all(6),
              //   child: Container(
              //     child: Text("BISMILLAH"),
              //   ),
              // child: Column(
              //   crossAxisAlignment: CrossAxisAlignment.center,
              //   mainAxisAlignment: MainAxisAlignment.center,
              //   children: <Widget>[
              //     Text("BISMILLAH"),
              //   ],
              // ),
              // ),
              // ),
              // Text(
              //   'Preset',
              //   style: TextStyle(
              //     fontSize: 30,
              //     // fontWeight: FontWeight.bold,
              //   ),
              // ),
              Padding(
                padding: EdgeInsets.only(top: 20),
                child: Center(
                  child: Text(
                    'PRESET',
                    style: TextStyle(
                      fontWeight: FontWeight.bold,
                      fontSize: 25,
                    ),
                  ),
                ),
              ),
              Padding(
                padding: EdgeInsets.only(top: 10, bottom: 20),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.spaceAround,
                  children: <Widget>[
                    FloatingActionButton(
                      child: Icon(
                        Icons.remove,
                        color: Colors.black,
                        size: 35,
                      ),
                      // color: Colors.black),
                      onPressed: () => setState(() {
                        _preset -= 1;
                        if (_preset < 0) {
                          _preset = 9;
                        }
                        // requenst preset mana yang akan di inisialisasi
                        requestPreset(_preset);
                      }),
                    ),
                    Text(
                      '$_preset',
                      style: TextStyle(
                        fontSize: 30,
                        fontWeight: FontWeight.bold,
                      ),
                    ),
                    FloatingActionButton(
                      child: Icon(
                        Icons.add,
                        color: Colors.black,
                        size: 35,
                      ),
                      onPressed: () => setState(() {
                        _preset += 1;
                        if (_preset > 9) {
                          _preset = 0;
                        }
                        // requenst preset mana yang akan di inisialisasi
                        requestPreset(_preset);
                      }),
                    ),
                  ],
                ),
              ),
              Padding(
                padding:
                    EdgeInsets.only(top: 50, bottom: 50, left: 20, right: 20),
                child: FloatingActionButton(
                  shape: StadiumBorder(),
                  child: Text(
                    'Copy Preset',
                    style: TextStyle(
                      fontSize: 20,
                      fontWeight: FontWeight.bold,
                      color: Colors.black,
                    ),
                  ),
                  onPressed: () {},
                ),
              ),
              Padding(
                padding:
                    EdgeInsets.only(top: 50, bottom: 50, left: 20, right: 20),
                child: FloatingActionButton(
                    shape: StadiumBorder(),
                    child: Text(
                      'Save to EEPROM',
                      style: TextStyle(
                        fontSize: 20,
                        fontWeight: FontWeight.bold,
                        color: Colors.black,
                      ),
                    ),
                    onPressed: () {
                      streamTransmit.add("SP\n");
                    }),
              ),
            ],
          ))),
    );
  }

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
                              value: l_gain[0],
                              min: -24,
                              max: 24,
                              divisions: 96,
                              // onChanged: null,
                              onChanged: (value) => setState(() {
                                _band = 0;
                                this.l_gain[_band] = value;
                                writePreset(_preset, _band, 0);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          l_gain[0].toString(),
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
                          value: l_fc[0],
                          min: 50,
                          max: 16000,
                          step: setStep(l_fc[0]),
                          onChanged: (value) => setState(() {
                            _band = 0;
                            this.l_fc[_band] = value;
                            writePreset(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          l_fc[0].toString(),
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
                              value: l_gain[1],
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (value) => setState(() {
                                _band = 1;
                                this.l_gain[_band] = value;
                                writePreset(_preset, _band, 0);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          l_gain[1].toString(),
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
                          value: l_fc[1],
                          min: 50,
                          max: 16000,
                          step: setStep(l_fc[1]),
                          onChanged: (value) => setState(() {
                            _band = 1;
                            this.l_fc[1] = value;
                            writePreset(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          l_fc[1].toString(),
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
                          value: l_bw[0],
                          min: 0,
                          max: 100,
                          step: setStep(l_bw[0]),
                          onChanged: (value) => setState(() {
                            _band = 1;
                            this.l_bw[_band - 1] = value;
                            writePreset(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          l_bw[0].toString(),
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
                              value: l_gain[2],
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (value) => setState(() {
                                _band = 2;
                                this.l_gain[_band] = value;
                                writePreset(_preset, _band, 0);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          l_gain[2].toString(),
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
                          value: l_fc[2],
                          min: 50,
                          max: 16000,
                          step: setStep(l_fc[2]),
                          onChanged: (value) => setState(() {
                            _band = 2;
                            this.l_fc[_band] = value;
                            writePreset(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          l_fc[2].toString(),
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
                          value: l_bw[1],
                          min: 0,
                          max: 100,
                          step: setStep(l_bw[1]),
                          onChanged: (value) => setState(() {
                            this.l_bw[1] = value;
                            _band = 2;
                            writePreset(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          l_bw[1].toString(),
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
                              value: l_gain[3],
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (value) => setState(() {
                                this.l_gain[3] = value;
                                _band = 3;
                                writePreset(_preset, _band, 0);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          l_gain[3].toString(),
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
                          value: l_fc[3],
                          min: 50,
                          max: 16000,
                          step: setStep(l_fc[3]),
                          onChanged: (value) => setState(() {
                            this.l_fc[3] = value;
                            _band = 3;
                            writePreset(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          l_fc[3].toString(),
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
                          value: l_bw[2],
                          min: 0,
                          max: 100,
                          step: setStep(l_bw[2]),
                          onChanged: (value) => setState(() {
                            this.l_bw[2] = value;
                            _band = 3;
                            writePreset(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          l_bw[2].toString(),
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
                              value: l_gain[4],
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (value) => setState(() {
                                this.l_gain[4] = value;
                                _band = 4;
                                writePreset(_preset, _band, 0);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          l_gain[4].toString(),
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
                          value: l_fc[4],
                          min: 50,
                          max: 16000,
                          step: setStep(l_fc[4]),
                          onChanged: (value) => setState(() {
                            this.l_fc[4] = value;
                            _band = 4;
                            writePreset(_preset, _band, 0);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          l_fc[4].toString(),
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
                            writePreset(_preset, _band, 0);
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
                          onChanged: (value) => setState(() {
                            this.r_level = value;
                            writePreset(_preset, _band, 1);
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
                              value: r_gain[0],
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (value) => setState(() {
                                this.r_gain[0] = value;
                                _band = 0;
                                writePreset(_preset, _band, 1);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          r_gain[0].toString(),
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
                          value: r_fc[0],
                          min: 50,
                          max: 16000,
                          step: setStep(r_fc[0]),
                          onChanged: (value) => setState(() {
                            this.r_fc[0] = value;
                            _band = 0;
                            writePreset(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          r_fc[0].toString(),
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
                              value: r_gain[1],
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (value) => setState(() {
                                this.r_gain[1] = value;
                                _band = 1;
                                writePreset(_preset, _band, 1);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          r_gain[1].toString(),
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
                          value: r_fc[1],
                          min: 50,
                          max: 16000,
                          step: setStep(r_fc[1]),
                          onChanged: (value) => setState(() {
                            this.r_fc[1] = value;
                            _band = 1;
                            writePreset(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          r_fc[1].toString(),
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
                          value: r_bw[0],
                          min: 0,
                          max: 100,
                          step: setStep(r_bw[0]),
                          onChanged: (value) => setState(() {
                            this.r_bw[0] = value;
                            _band = 1;
                            writePreset(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          r_bw[0].toString(),
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
                              value: r_gain[2],
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (value) => setState(() {
                                this.r_gain[2] = value;
                                _band = 2;
                                writePreset(_preset, _band, 1);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          r_gain[2].toString(),
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
                          value: r_fc[3],
                          min: 50,
                          max: 16000,
                          step: setStep(r_fc[3]),
                          onChanged: (value) => setState(() {
                            this.r_fc[3] = value;
                            _band = 2;
                            writePreset(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          r_fc[3].toString(),
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
                          value: r_bw[1],
                          min: 0,
                          max: 100,
                          step: setStep(r_bw[1]),
                          onChanged: (value) => setState(() {
                            this.r_bw[1] = value;
                            _band = 2;
                            writePreset(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          r_bw[1].toString(),
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
                              value: r_gain[3],
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (value) => setState(() {
                                this.r_gain[3] = value;
                                _band = 3;
                                writePreset(_preset, _band, 1);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          r_gain[3].toString(),
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
                          value: r_fc[3],
                          min: 50,
                          max: 16000,
                          step: setStep(r_fc[3]),
                          onChanged: (value) => setState(() {
                            this.r_fc[3] = value;
                            _band = 3;
                            writePreset(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          r_fc[3].toString(),
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
                          value: r_bw[2],
                          min: 0,
                          max: 100,
                          step: setStep(r_bw[2]),
                          onChanged: (value) => setState(() {
                            this.r_bw[2] = value;
                            _band = 3;
                            writePreset(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          r_bw[2].toString(),
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
                              value: r_gain[4],
                              min: -24,
                              max: 24,
                              divisions: 96,
                              onChanged: (value) => setState(() {
                                this.r_gain[4] = value;
                                _band = 4;
                                writePreset(_preset, _band, 1);
                              }),
                            ),
                          ),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          r_gain[4].toString(),
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
                          value: r_fc[4],
                          min: 50,
                          max: 16000,
                          step: setStep(r_fc[4]),
                          onChanged: (value) => setState(() {
                            this.r_fc[4] = value;
                            _band = 4;
                            writePreset(_preset, _band, 1);
                          }),
                        ),
                      ),
                      Align(
                        alignment: Alignment.bottomCenter,
                        child: Text(
                          r_fc[4].toString(),
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

// class MyDrawer extends StatelessWidget {
//   // final Function onTap;

//   // MyDrawer({this.onTap});
//   @override
//   Widget build(BuildContext context) {
//     return
//   }
// }

// class CustomTrackShape extends RoundedRectSliderTrackShape {
//   Rect getPreferredRect({
//     @required RenderBox parentBox,
//     Offset offset = Offset.zero,
//     @required SliderThemeData sliderTheme,
//     bool isEnabled = false,
//     bool isDiscrete = false,
//   }) {
//     final double trackHeight = sliderTheme.trackHeight;
//     final double trackLeft = offset.dx;
//     final double trackTop =
//         offset.dy + (parentBox.size.height - trackHeight) / 2;
//     final double trackWidth = parentBox.size.width;
//     return Rect.fromLTWH(trackLeft, trackTop, trackWidth * 2, trackHeight);
//     // return Rect.fromLTWH(0, 0, 2, 2);
//   }
// }

// class CustomTrackShape2 extends RoundedRectSliderTrackShape {
//   Rect getPreferredRect({
//     @required RenderBox parentBox,
//     Offset offset = Offset.zero,
//     @required SliderThemeData sliderTheme,
//     bool isEnabled = false,
//     bool isDiscrete = false,
//   }) {
//     final double trackHeight = sliderTheme.trackHeight;
//     final double trackLeft = offset.dx - 20;
//     final double trackTop =
//         offset.dy + (parentBox.size.height - trackHeight) / 2;
//     final double trackWidth = parentBox.size.width;
//     // print('left $trackWidth');
//     return Rect.fromLTWH(trackLeft, trackTop, trackWidth + 40, trackHeight);
//   }
// }
