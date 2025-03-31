# cifar10-cnn_accelerator

A CNN accelerator for CIFAR-10 implemented on the Zybo Z7-10 FPGA.  
This project includes system modeling in SystemC, RTL design in VHDL, functional verification with UVM, and Linux driver development.

## Repository Structure

- **cosim**: Co-simulation files (SystemC testbench and RTL model of the accelerator), using Cadence Xcelium.
- **data**: All necessary input data for the CNN and expected output for golden-vector UVM verification.
- **docs**: Project documentation (written in Serbian).
- **driver_app**: Linux driver and application for running the network on the Zybo Z7-10 FPGA board.
  - **driver**: Driver for CNN convolution layers.
  - **app**: Application for running the network on the Zybo Z7-10 FPGA board.
- **rtl**: RTL implementation of the accelerator, executing three convolution layers.
- **specification**: Project specification, including C++ and Python implementations of the CIFAR-10 CNN network and bit-accurate analysis using SystemC.
- **tb**: RTL testbenches.
- **uvm**: UVM environment for functional verification of the accelerator.
- **vp**: Virtual platform implemented using a SystemC model.
