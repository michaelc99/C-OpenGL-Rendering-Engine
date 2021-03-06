#ifndef MATH_EXCEPTION_H
#define MATH_EXCEPTION_H

#include "general_exception.h"

namespace Engine {

class MathException : public GeneralException {
    public:
        MathException(std::string message) : GeneralException(std::string("MathException:") + message) {}
    private:
};

class DivideByZeroException : public MathException {
    public:
        DivideByZeroException(std::string message) : MathException(std::string("DivideByZeroException:") + message) {}
    private:
};

};

#endif //MATH_EXCEPTION_H
