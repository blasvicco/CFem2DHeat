# CFem2DHeat

This project is a GID Problem Type distributed under GPL V3.

I developed this project with a didactic purpose to help anyone in the engineer field to understand the basis of the Finite Element Method (FEM) and its application.

A simple physic model of heat transfer was chossen to keep the code simple and legible for software engineers that were taken their first steps in Numerical Processing.

In any case, a minimum of knowledge in the involved fields are required. I will assume that you know about Thermodynamic Principles, Differential Equations, the basis of Numerical Processing, Continuous Mechanic and stuff like that.

## What's GID?
"GiD has been designed to cover all the common needs in the numerical simulation field from pre to post processing: geometrical modeling, effective definition of analysis data, meshing, data transfer to analysis software, as well as the visualization of numerical results."
Please refer to the next [link](https://www.gidhome.com/whats-gid/) for more details.

## What's a GID Problem Type?
"When GiD is to be used for a particular type of analysis, it is necessary to predefine all the information required from the user and to define the way the final information is given to the solver module. To do so, some files are used to describe conditions, materials, general data, units systems, symbols and the format of the input file for the solver. We call Problem Type to this collection of files used to configure GiD for a particular type of analysis."
Please refer to the next [link](http://www-opale.inrialpes.fr/Aerochina/info/en/html-version/gid_16.html) for more details.

## What's this project contain?
You will find in this project:

-. The source code of the FEM implementation for the Heat Transfer analysis in a 2D geometry.
The module called CFem2DHeat was developed using C++ under XCode, following Object Oriented Programming (OOP) and [GNU Scientific Library (GSL)](https://www.gnu.org/software/gsl/).

-. The folder with all the needed files to define the GPT (GID Problem Type) plus the binary CFem2DHeat compiled for OSX.

-. 2 Examples of Heat Transfer problems. One with a mix of fixed temperature and flux boundary conditions and the other with a mix of fixed temperatures and convection boundary conditions.

## What am I reading?
In this document I will exaplin in detail about the implementation of the CFem2DHeat Module from a Software Engineer perspective.

I will try to avoid entering in the deep universe of Physics and Geometrical Math that are involved in the FEM, but I will highly recommend you to read some of the vast bibliography you can find online.
This methodology can be traced back to the early 1940s, so many reseachs and works were made in this field.

...IN PROCESS...
