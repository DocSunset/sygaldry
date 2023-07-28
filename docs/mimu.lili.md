# MIMU

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

A magnetic-inertial measurement unit (MIMU), also called a "magnetic, angular
rate, and gravity sensor" (MARG), is a combination of 9 sensors: three
orthogonal magnetic field sensors, three orthogonal angular rate sensors, and
three orthogonal accelerometers. Or in other words, it is a combination of
three 3-axis sensor triads. MIMUs are used to estimate the orientation of
the object to which they are attached, using sensor fusion to combine the
measurements from all of the sensors in order to improve the estimate.

# Why These Sensors?

## Accelerometer

- Can't seperate gravity from movement
- Can't resolve yaw angle

The accelerometer alone provides substantial information about the pose and
movement of the device, but has strict limitations. An accelerometer measures
acceleration due to movement *and* gravity, and there is no way to seperate
these two measurement components without additional information. This means
that, if the device is presumed to be relatively motionless, the acceleration
due to gravity can be used as a measurement of the global up/down axis
expressed in the sensor frame of reference. This can be used to resolve two
rotational degrees of freedom (e.g. pitch and roll). However, if the device is
moving then the accerometer data cannot be used for orientation estimation.
Furthermore, regardless of the device's movement, it's not possible to fully
estimate the orientation of the device from the accelerometer alone. It will
tell you which way is up, but not e.g. whether you are facing north or south.

## Gyroscope

- Sensor bias integration drift
- Only provides relative orientation

The gyroscope provides a measurement of angular rate. By integrating its
measurements over time, change in the orientation of the device can be tracked
relative to its starting orientation. Given a known starting orientation and an
ideal gyroscope, no other sensor is necessary to track orientation.
Unfortunately, actual gyroscopes are generally not ideal, and the MEMS
gyroscopes used in consumer MIMUs are generally among the worst on the market.
The main issue is sensor bias caused by e.g. manufacturing variability and
thermal expansion and contraction of the sensing elements. This bias tends to
slowly change over time, making it somewhat tricky to compensate. But if it is
not removed from the measurement, then the bias error accumulates rapidly in
the integration process, resulting in an orientation estimate that drifts
further and further from reality as long as the measurements from the gyroscope
are integrated.

## Magnetometer

- Can't resolve pitch and roll
- Noisy low-rate measurements poor for interactive use

The magnetometer measures the local magnetic field. In a MIMU, the field of
interest is the Earth's magnetic field. This is generally measured by the
sensor as a vector that points in the direction of magnetic north, usually with
a significant tilt up or down depending on whether the measurement is taken
north or south of the equator. This tilt is called the magnetic dip or magnetic
inclination. A positive dip angle represents a tilt downward toward the ground,
and is found in the norther hemisphere. A negative dip angle represents a tilt
up towards the sky, and is found in the southern hemisphere. If the
magnetometer is assumed to be aligned with the horizontal plane, e.g. if it
were strapped to a vehicle, then the dip angle can be compensated, and the
magnetometer's measurement of the Earth's magnetic field can be used to resolve
two rotational degrees of freedom (e.g. pitch and yaw). However, if the
magnetometer can be tilted arbitrarily, then it may not be possible to seperate
the tilt due to motion and magnetic dip. Furthermore, magnetometer measurements
tend to have a lower sampling rate and higher noise. Using such measurements in
isolation tends not to provide good temporal responsiveness necessary for
interactive applications.

## Intertial measurement units

- Addresses gyro bias and motion acceleration
- Still can't resolve yaw angle

An inertial measurement unit (IMU) combines an accelerometer and gyroscope.
This enables the gyroscope to be relied on when the device is moving,
overcoming the issue of seperating accelerations due to gravity and movement.
Further, the accelerometer provides a reference when the device is stationary
(which e.g. can be detected with the gyroscope or the derivative of the
accelerometer measurement), allowing gyroscope bias integration drift to be
compensated and the gyroscope bias estimated. The combination is more robust
that either sensor alone. However, an IMU is not able to provide an estimate
of device orientation in the global frame of reference, because neither the
gyroscope nor the accelerometer can resolve the device yaw angle.

## Tilt-compensated digital compasses

- Provides full global orientation estimate
- Still subject to gravity/motion acceleration issue
- Still subject to magnetometer noise and sampling rate issue

Combining a magnetometer with an accelerometer is regularly used to develop
tilt-compensated compass applications. Under the assumption of low acceleration
due to movement, the accelerometer can be used to compensate tilt in the
measurement platform, allowing the dip angle to be removed from the
magnetometer measurement and giving a complete characterization of the
orientation of the sensor with respect to the global frame of reference.
However, digital compasses of this kind are still vulnerable to distortions
caused by significant motion, and remain subject to the limitations of the
magnetometer measurement in terms of noise and sampling rate.

## MIMU

- Full global orientation estimation
- Full gyro bias integration drift compensation
- Can discriminate acceleration due to gravity from that due to motion
- Excellent temporal characteristics for interactive applications

Combining these three types of sensors provides an excellent complementary
combination of capabilities, allowing most of the issues with each individual
sensor to be compensated for. The gyroscope and accelerometer have excellent
temporal measurement characteristics, enabling low-latency interactive
applications. The gyroscope and magnetometer together allow to seperate the
accelerations due to gravity and due to motion. When the device is moving,
the gyroscope and magnetometer can be relied on to update the orientation of
the device. At all times, the magnetometer can partially compensate for
bias integration drift from the gyroscope. When stationary, the accelerometer
and magnetometer together provide a complete orientation estimate that
can further compensate gyro bias integration drift.
