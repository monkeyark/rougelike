#include <cstdlib>

#include "dice.h"

# define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

int32_t dice::roll(void) const
{
    int32_t total;
    uint32_t i;

    total = base;

    if (sides)
    {
        for (i = 0; i < number; i++)
        {
            total += rand_range(1, sides);
        }
    }

    return total;
}

std::ostream &dice::print(std::ostream &o)
{
    return o << base << '+' << number << 'd' << sides;
}

std::ostream &operator<<(std::ostream &o, dice &d)
{
    return d.print(o);
}

std::string dice::print_string()
{
    return std::to_string(base) + "+" + std::to_string(number) + "d" + std::to_string(sides);
}

