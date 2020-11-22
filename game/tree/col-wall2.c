/* wall2 collision */

Collider col_wall2;
ColliderMake( &col_wall2 );

col_wall2.super.task = NULL_TASK;

/* mass in kilograms */
col_wall2.m = 1.0e9;

/* bounding box corners in meters */
col_wall2.min[0] = -0.01;
col_wall2.min[1] = -9.95;
col_wall2.min[2] = -9.95;

col_wall2.max[0] = 0.01;
col_wall2.max[1] = 9.95;
col_wall2.max[2] = 9.95;

/* pointers to kinematic data */
col_wall2.x = kin_wall2.x;
col_wall2.v = kin_wall2.v;
