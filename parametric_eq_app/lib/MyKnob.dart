import 'package:flutter/material.dart';
import 'dart:math';
import 'package:flutter/services.dart';

class MyKnob extends StatefulWidget {
  // Define the parameters of this widget
  final double value;
  final double min;
  final double max;
  final double step;

  // Two extra parameters to make the widget more easy to customise
  final double size;
  final Color color;

  // ValueChanged is a type built into Dart for a function that changes a value
  final ValueChanged<double> onChanged;

  // Define a build method for the widget which uses these parameters
  MyKnob(
      {this.value,
      this.min = 0,
      this.max = 1,
      this.step = 1,
      this.color = Colors.blue,
      this.size = 50,
      this.onChanged});

  @override
  State<StatefulWidget> createState() => MyKnobState();
}

class MyKnobState extends State<MyKnob> {
  // These are static constants because they are in internal parameters of the knob that
  // can't be changed from the outside
  static const double minAngle = -160;
  static const double maxAngle = 160;
  static const double sweepAngle = maxAngle - minAngle;

  @override
  void initState() {
    super.initState();
    SystemChrome.setPreferredOrientations([
      DeviceOrientation.landscapeRight,
      DeviceOrientation.landscapeLeft,
    ]);
  }

  @override
  Widget build(BuildContext context) {
    // distanceToAngle now depends on potentially non-constant parameters,
    // so must be defined in the build method
    // double distanceToAngle = 0.1 * (widget.max - widget.min);
    double distanceToAngle = widget.step;

    double _normalisedValue =
        (widget.value - widget.min) / (widget.max - widget.min);
    // double _normalisedValue = (widget.value - widget.min) / 179;
    double _angle = (minAngle + _normalisedValue * sweepAngle) * 2 * pi / 360;
    double size = widget.size;
    return Center(
      child: Container(
        width: size,
        height: size,
        child: GestureDetector(
          onHorizontalDragUpdate: (DragUpdateDetails details) {
            double changeInX = details.delta.dx;
            double changeInValue = distanceToAngle * changeInX.round();
            double newValue = widget.value + changeInValue;
            double clippedValue = min(max(newValue, widget.min), widget.max);
            widget.onChanged(clippedValue);
          },
          child: Transform.rotate(
            angle: _angle,
            child: ClipOval(
                child: Container(
                    color: Color.fromARGB(255, 105, 21, 102),
                    child: Icon(
                      Icons.all_out,
                      color: Colors.black,
                      size: size,
                    ))),
          ),
        ),
      ),
    );
  }
}
