L0 = 0.3;
L1 = 0.2;
L2 = 0.1;
t0 = 0.4;
t1 = 0.6;
t2 = 1.2;
t0d = rad2deg(t0);
t1d = rad2deg(t1);
t2d = rad2deg(t2);
R1 = [cosd(t0d) -sind(t0d) 0 0;
      sind(t0d)  cosd(t0d) 0 0;
      0            0       1 0;
      0            0       0 1];
R2 = [cosd(t1d) -sind(t1d) 0 0;
      sind(t1d)  cosd(t1d) 0 0;
      0            0       1 0;
      0            0       0 1];
R3 = [cosd(t2d) -sind(t2d) 0 0;
      sind(t2d)  cosd(t2d) 0 0;
      0            0       1 0;
      0            0       0 1];
D1 = [eye(4,3) [L0;0;0;1]];
D2 = [eye(4,3) [L1;0;0;1]];
D3 = [eye(4,3) [L2;0;0;1]];

T1 = R1*D1;
T2 = R2*D2;
T3 = R3*D3;
T = T1*T2*T3

xe = L0*cosd(t0d)+L1*cosd(t0d+t1d)+L2*cosd(t0d+t1d+t2d)
ye = L0*sind(t0d)+L1*sind(t0d+t1d)+L2*sind(t0d+t1d+t2d)
t = t0d + t1d + t2d