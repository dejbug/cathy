NAME := cathy
LIB := Bitmap Child Dialog Edit Event File Font String Strings Thread Window
WINLIBS := gdi32 comctl32 comdlg32 ws2_32 winmm msimg32
SYMBOLS := WIN32_LEAN_AND_MEAN STRICT _WIN32_WINNT=0x0400 _WIN32_WINNT=0x0400
HEADERS := BrowserControl.h Button.h Chessboard.h Chessfont.h Clock.h Dragger.h Editbox.h MoveHistory.h Operator.h ProblemInfoBox.h resource.h Stack.h Static.h Time.h Timer.h Trainer.h Worker.h

CXXFLAGS := -Wall -Wextra -Wpedantic -O3
CXXFLAGS += -fpermissive
CXXFLAGS += -Wno-unused-parameter -Wno-parentheses
CXXFLAGS += $(SYMBOLS:%=-D%)

LDFLAGS := -Xlinker --subsystem=windows
LDFLAGS += $(WINLIBS:%=-l%)

$(NAME).exe : Main.o resource.o $(LIB:%=%.o) $(HEADERS)
Main.o : Main.cpp
$(LIB:%=%.o) : %.o : lib/%.cpp

resource.o : resource.rc resource.h ; windres $< $@

%.exe : ; $(CXX) -o $@ $(filter %.o %.a %.dll,$^) $(CXXFLAGS) $(LDFLAGS)
%.o : ; $(CXX) -o $@ -c $(filter %.cpp,$^) $(CXXFLAGS)

.PHONY : run clean reset
run : $(NAME).exe ; @.\$<
clean : ; DEL *.o 2>NUL
reset : | clean ; DEL $(NAME).exe 2>NUL
