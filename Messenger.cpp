
#include "Messenger.h"

Messenger::Messenger(Elegoo_TFTLCD *screen, TouchScreen *ts, VKeys *keys) {
    _screen = screen;
    _ts = ts;
    _keys = keys;
}