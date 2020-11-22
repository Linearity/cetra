/* floor collision */

Collider col_floor;
ColliderMake( &col_floor );

col_floor.super.task = NULL_TASK;

/* mass in kilograms */
col_floor.m = 1.0e9;

/* bounding box corners in meters */
col_floor.min[0] = -9.95;
col_floor.min[1] = -0.01;
col_floor.min[2] = -9.95;

col_floor.max[0] = 9.95;
col_floor.max[1] = 0.01;
col_floor.max[2] = 9.95;

/* pointers to kinematic data */
col_floor.x = kin_floor.x;
col_floor.v = kin_floor.v;
