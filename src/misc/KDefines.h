#ifndef KDEFINES_H
#define KDEFINES_H

//std::string to QString
#define K_SQ_STR(stdstring) QString::fromStdString(stdstring)

#define K_CONTINUE_IF(condition) if((condition)) continue;
#define K_CONTINUE_IF_NOT(condition) K_CONTINUE_IF(!(condition))
#define K_BREAK_IF(condition) if((condition)) break;
#define K_BREAK_IF_NOT(condition) K_BREAK_IF(!(condition))
#define K_RETURN_IF(condition) if((condition)) return;
#define K_RETURN_IF_NOT(condition) K_RETURN_IF(!(condition))

#endif // KDEFINES_H
