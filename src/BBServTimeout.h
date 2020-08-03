// BBServTimeout.h

#pragma once

#include <iostream>
#include <string>
#include "BBServException.h"

/**
 *Exception type indicating a timeout situation and accepting an error text upon creation.
 */
class BBServTimeout : public BBServException
{
    public:
        BBServTimeout(const std::string_view& text)
            : BBServException(text)
        {
        }

        /**
         *Return a descriptive text explaining the exceptional situation.
         */
        virtual const char* what() const noexcept
        {
            return BBServException::what();
        }
};
