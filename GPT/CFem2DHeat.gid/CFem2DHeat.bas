==================================================================
                        General Data File
==================================================================
Geometry Unity:
*GenData(1)

Number of Elements & Nodes:
*nelem *npoin

.................................................................

Begin Materials
N. Materials = *nmats
      Mat.          k_coef._(W/m/C°)     h_coef._(W/mˆ2/C°)
*loop materials
*format "%10i%20.5e%20.5e"
*set var PROP1(real)=Operation(MatProp(k_coef._(W/m/C°),real))
*set var PROP2(real)=Operation(MatProp(h_coef._(W/mˆ2/C°),real))
*MatNum *PROP1 *PROP2
*end materials

.................................................................

Point conditions
*set Cond Point_Temperature *nodes
*set var NFIX(int)=CondNumEntities(int)
*NFIX
Temperature
      Node  Temp(C°)
*loop nodes *OnlyInCond
*format "%10i%13.5e"
*NodesNum *cond(1)
*end

.................................................................

Line conditions
*set Cond Line_Temperature *nodes
*set var NFIX(int)=CondNumEntities(int)
*NFIX
Temperature
      Node  Temp(C°)
*loop nodes *OnlyInCond
*format "%10i%13.5e"
*NodesNum *cond(1)
*end

.................................................................

Line conditions
*set Cond Line_Flux *nodes
*set var NFIX(int)=CondNumEntities(int)
*NFIX
Flux
      Node  Flux(W/mˆ2)
*loop nodes *OnlyInCond
*format "%10i%13.5e"
*NodesNum *cond(1)
*end

.................................................................

Line conditions
*set Cond Line_Convection *nodes
*set var NFIX(int)=CondNumEntities(int)
*NFIX
Convection
      Node  Temp(C°)
*loop nodes *OnlyInCond
*format "%10i%13.5e"
*NodesNum *cond(1)
*end

.................................................................

Coordinates:
      Node    X              Y
*set elems(all)
*loop nodes
*format "%10i%14.5e%14.5e"
*NodesNum *NodesCoord(1,real) *NodesCoord(2,real)
*end nodes

.................................................................

Connectivities:
   Element    Node(1)   Node(2)   Node(3)    Material
*loop elems
*format "%10i%10i%10i%10i%10i"
*ElemsNum *ElemsConec *ElemsMat 
*end elems
