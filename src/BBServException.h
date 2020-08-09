// BBServException.h

#pragma once

#include <iostream>
#include <exception>
#include <string>

/**
 *Exception type accepting an error text upon creation.
 */
class BBServException : public std::exception
{
    std::string errorText;

    public:
        BBServException(const std::string_view& text)
            : std::exception()
            , errorText(text)
        {
        }

        /**
         *Return a descriptive text explaining the exceptional situation.
         */
        virtual const char* what() const noexcept
        {
            return this->errorText.data();
        }
};
