#ifndef GLOBSTR_H
#define GLOBSTR_H

#define SENSORDATA_SIGN 'S'
#define ACTION_SIGN 'A'
#define TCP_PORT 1235

//#define MODE_PAUSE  0
#define MODE_HOME   1
#define MODE_SCAN   2
#define MODE_SCAN_X 3
#define MODE_MOVE   4
#define CNC_STOP    5
#define CNC_PAUSE   6
#define CNC_RESUME  7
#define MODE_CALIBRATION 8

#define CNC_LENGHT   1170 // мм
#define CNC_CARRIAGE 690 //мм

typedef struct{
    unsigned int mode;    // 4 байт       // режим работы 1 - радиальное сканирование, 2 - только по x, 3 - вернуть х в начало
    unsigned int x;       // 4 байта      //  перемещение по оси x
    //unsigned int stepX;   // 4 байта      // шаг чтения датчиком x
    //unsigned int stepA;   // 4 байта      // шаг чтения датчиком а (в градусах)
} CncAction;

typedef struct {
    double x;
    double angle;
    double sensorBr;
    double sensorBz;
} SensorData;

#endif // CONSTANTS_H
