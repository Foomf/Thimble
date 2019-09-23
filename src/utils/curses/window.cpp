#include "utils/curses/window.hpp"

#include <curses.h>

namespace blyss::utils::curses
{
    window::window(std::shared_ptr<curses_state> curses_state, std::int32_t nlines, std::int32_t ncols, std::int32_t begin_y, std::int32_t begin_x)
        : curses_state_{std::move(curses_state)}
        , win_ptr_{curses_window_ptr(newwin(nlines, ncols, begin_y, begin_x), delwin)}
    {
    }

    void window::move_and_print(std::int32_t y, std::int32_t x, const std::string& str) const
    {
        ::mvwprintw(win_ptr_.get(), y, x, str.c_str());
    }

    std::int32_t window::get_char() const
    {
        return wgetch(win_ptr_.get());
    }

    void window::enable_reverse() const
    {
        wattron(win_ptr_.get(), A_REVERSE);
    }

    void window::disable_reverse() const
    {
        wattroff(win_ptr_.get(), A_REVERSE);
    }

    void window::refresh() const
    {
        wrefresh(win_ptr_.get());
    }

    void window::use_keypad() const
    {
        keypad(win_ptr_.get(), true);
    }

}