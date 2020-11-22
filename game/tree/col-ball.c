/* ball collision */

Collider col_ball;
ColliderMake( &col_ball );

col_ball.super.task = (Task)elastic_response;

/* mass in kilograms */
col_ball.m = 500.0;

/* bounding box corners in meters */
col_ball.min[0] = -0.8;
col_ball.min[1] = -0.8;
col_ball.min[2] = -0.8;

col_ball.max[0] = 0.8;
col_ball.max[1] = 0.8;
col_ball.max[2] = 0.8;

/* pointers to kinematic data */
col_ball.x = kin_ball.x;
col_ball.v = kin_ball.v;
