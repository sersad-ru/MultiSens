#pragma once

#define SSBUILDID_LENGTH 14 //длина строки с версией прошивки

// Тут куча зажигалова для получения ID сборки
#define SSBUILDID_Y1 (__DATE__[ 7])
#define SSBUILDID_Y2 (__DATE__[ 8])
#define SSBUILDID_Y3 (__DATE__[ 9])
#define SSBUILDID_Y4 (__DATE__[10])

#define SSBUILDID_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define SSBUILDID_IS_FEB (__DATE__[0] == 'F')
#define SSBUILDID_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define SSBUILDID_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define SSBUILDID_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define SSBUILDID_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define SSBUILDID_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define SSBUILDID_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define SSBUILDID_IS_SEP (__DATE__[0] == 'S')
#define SSBUILDID_IS_OCT (__DATE__[0] == 'O')
#define SSBUILDID_IS_NOV (__DATE__[0] == 'N')
#define SSBUILDID_IS_DEC (__DATE__[0] == 'D')

#define SSBUILDID_M1 ((SSBUILDID_IS_OCT || SSBUILDID_IS_NOV || SSBUILDID_IS_DEC) ? '1' : '0')

#define SSBUILDID_M2 ((SSBUILDID_IS_JAN) ? '1' : (SSBUILDID_IS_FEB) ? '2' : (SSBUILDID_IS_MAR) ? '3' : \
                      (SSBUILDID_IS_APR) ? '4' : (SSBUILDID_IS_MAY) ? '5' : (SSBUILDID_IS_JUN) ? '6' : \
                      (SSBUILDID_IS_JUL) ? '7' : (SSBUILDID_IS_AUG) ? '8' : (SSBUILDID_IS_SEP) ? '9' : \
                      (SSBUILDID_IS_OCT) ? '0' : (SSBUILDID_IS_NOV) ? '1' : (SSBUILDID_IS_DEC) ? '2' : '?' )

#define SSBUILDID_D1 ((__DATE__[4] >= '0') ? (__DATE__[4]) : '0')
#define SSBUILDID_D2 (__DATE__[5])

#define SSBUILDID_HR1 (__TIME__[0])
#define SSBUILDID_HR2 (__TIME__[1])

#define SSBUILDID_MN1 (__TIME__[3])
#define SSBUILDID_MN2 (__TIME__[4])

#define SSBUILDID_SC1 (__TIME__[6])
#define SSBUILDID_SC2 (__TIME__[7])

#define SSBUILDID_BUILD SSBUILDID_Y1, SSBUILDID_Y2, SSBUILDID_Y3, SSBUILDID_Y4, SSBUILDID_M1, SSBUILDID_M2,\
                        SSBUILDID_D1, SSBUILDID_D2, SSBUILDID_HR1, SSBUILDID_HR2, SSBUILDID_MN1,\
                        SSBUILDID_MN2, SSBUILDID_SC1, SSBUILDID_SC2, '\0'

const char __build_id[15] = {SSBUILDID_BUILD};
