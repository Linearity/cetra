/* wall1 collision */

Collider col_wall1;
ColliderMake( &col_wall1 );

col_wall1.super.task = NULL_TASK;

/* mass in kilograms */
col_wall1.m = 1.0e9;

/* bounding box corners in meters */
col_wall1.min[0] = -9.95;
col_wall1.min[1] = -9.95;
col_wall1.min[2] = -0.01;

col_wall1.max[0] = 9.95;
col_wall1.max[1] = 9.95;
col_wall1.max[2] = 0.01;

/* pointers to kinematic data */
col_wall1.x = kin_wall1.x;
col_wall1.v = kin_wall1.v;
