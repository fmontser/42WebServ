
MAKEFLAGS 		+=	--silent
MAKEFILE		:=	Makefile

INC_DIR			:=	include/
SRC_DIR			:=	src/
OBJ_DIR			:=	obj/
BIN_DIR			:=	bin/

NAME			:=	webserv

HDRS			:=	ServerConstants.hpp	Config.hpp  Route.hpp Server.hpp HttpMessage.hpp HttpRequest.hpp\
					HttpResponse.hpp ConnectionManager.hpp DataAdapter.hpp CgiProcessor.hpp\
					FileManager.hpp  SignalManager.hpp Connection.hpp HttpProcessor.hpp HttpHeader.hpp\
					HeaderValue.hpp HeaderProperty.hpp Utils.hpp

SRCS			:=	main.cpp			Config.cpp  Route.cpp Server.cpp HttpMessage.cpp HttpRequest.cpp\
					HttpResponse.cpp ConnectionManager.cpp DataAdapter.cpp CgiProcessor.cpp\
					FileManager.cpp SignalManager.cpp Connection.cpp HttpProcessor.cpp HttpHeader.cpp\
					HeaderValue.cpp HeaderProperty.cpp Utils.cpp

OBJS			:=	$(SRCS:.cpp=.o)
INPUT			:=	"../config/config.cfg"

CC				:=	g++ #c++
CC_FLAGS		:=	-Wall -Werror -Wextra -g -c -std=c++98
CLEAN_TARGETS	:=	$(wildcard $(addprefix $(OBJ_DIR), $(OBJS) $(TEST_OBJS)))
FCLEAN_TARGETS	:=	$(wildcard $(addprefix $(BIN_DIR), $(NAME) $(TEST_NAME)))\
					$(wildcard $(addprefix $(RES_DIR), $(RRSS)))

COLOR_GREEN		:=	\033[0;32m
COLOR_RED		:=	\033[0;31m
COLOR_BLUE		:=	\033[0;34m
COLOR_END		:=	\033[0m

vpath %.hpp $(INC_DIR)
vpath %.cpp $(SRC_DIR)
vpath %.o $(OBJ_DIR)
vpath % $(BIN_DIR)

all: $(NAME)

$(NAME): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(addprefix $(OBJ_DIR),$(OBJS)) -o $(BIN_DIR)$(NAME)
	@echo "$(COLOR_GREEN)write file: $(BIN_DIR)$(NAME)$(COLOR_END)"

%.o : %.cpp $(HDRS) $(MAKEFILE)
	@mkdir -p $(OBJ_DIR)
	@$(CC) -I $(INC_DIR) $(CC_FLAGS) $< -o $(OBJ_DIR)$@
	@echo "$(COLOR_GREEN)write file: $(OBJ_DIR)$@ $(COLOR_END)"

run: all
	@cd $(BIN_DIR) && ./$(NAME) $(INPUT)

valgrind: all
	@cd $(BIN_DIR) && valgrind ./$(NAME) $(INPUT)

clean:
	@$(foreach item,$(CLEAN_TARGETS),echo "$(COLOR_RED)delete file: $(item)$(COLOR_END)"; rm $(item);)

fclean: clean
	@$(foreach item,$(FCLEAN_TARGETS),echo "$(COLOR_RED)delete file: $(item)$(COLOR_END)"; rm $(item);)
	
re: fclean all

.PHONY: all clean fclean re