# VitalAI — Real-Time IoT Health Monitoring System

## Overview

VitalAI is a real-time IoT-based health monitoring and telemetry system developed using ESP32 and MAX30102.

The system wirelessly streams physiological data to a browser-based dashboard using WebSocket communication and JSON telemetry packets.

## Features

* Real-time heart-rate monitoring
* SpO2 visualization
* Wireless telemetry streaming
* Browser-based live dashboard
* WebSocket communication
* JSON data exchange
* Waveform visualization
* Session monitoring

## Physiological Metrics Implemented

* Heart Rate (HR)
* Blood Oxygen Saturation (SpO2)
* Heart Rate Variability (HRV)
* Stress Index Estimation
* Cardiac Load Monitoring
* Perfusion-Level Visualization
* Real-Time Telemetry Dashboard


## Hardware Components

* ESP32
* MAX30102 Pulse Oximeter Sensor

## Software Technologies

* Embedded C
* HTML/CSS/JavaScript
* WebSocket
* JSON

## Communication Protocols

* I2C
* WebSocket

## System Architecture

MAX30102 → ESP32 → WebSocket → Browser Dashboard

## Concepts Applied

* Embedded Systems
* Wireless Communication
* Real-Time Telemetry
* Sensor Interfacing
* IoT Systems
* Browser-Based Monitoring

## Future Improvements

* Real signal filtering
* Cloud integration
* LoRa communication
* Mobile app support
* AI-assisted analytics

Note: Hardware setup was submitted to the institute as part of project evaluation; repository contains implementation code, dashboard interface, and system architecture.
