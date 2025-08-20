#ifndef ABSTRACT_WIDGETS_HPP_
#define ABSTRACT_WIDGETS_HPP_

namespace widgets {
struct widget {
    [[nodiscard]] virtual int width() const = 0;
    [[nodiscard]] virtual int height() const = 0;

    widget(const widget &) = delete;
    widget(widget &&) = delete;
    widget &operator=(const widget &) = delete;
    widget &operator=(widget &&) = delete;

protected:
    widget() = default;
};

struct container : widget {
    virtual void update_layout() = 0;
};
}  // namespace widgets

#endif  // ABSTRACT_WIDGETS_HPP_
