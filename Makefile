
MAKEFLAGS 		+=	--silent
MAKEFILE		:=	Makefile

INC_DIR			:=	include/
SRC_DIR			:=	src/
OBJ_DIR			:=	obj/
BIN_DIR			:=	bin/
CGI_DIR			:=	$(SRC_DIR)cgi-src/
CGI_BIN_DIR		:=	cgi-bin/

NAME			:=	webserv
CGI_BIN_GET		:=	multiply.cgi
CGI_BIN_POST	:=	replace.cgi
CGI_BIN_TEST 			:=	cgi_test.cgi

HDRS			:=	ServerConstants.hpp	Config.hpp Socket.hpp Route.hpp Server.hpp HttpMessage.hpp HttpRequest.hpp\
					HttpResponse.hpp ConnectionManager.hpp DataAdapter.hpp CgiAdapter.hpp\
					FileManager.hpp  SignalManager.hpp Connection.hpp HttpProcessor.hpp HttpHeader.hpp\
					HeaderValue.hpp HeaderProperty.hpp Utils.hpp Index.hpp PathManager.hpp

SRCS			:=	main.cpp			Config.cpp Socket.cpp Route.cpp Server.cpp HttpMessage.cpp HttpRequest.cpp\
					HttpResponse.cpp ConnectionManager.cpp DataAdapter.cpp CgiAdapter.cpp\
					FileManager.cpp SignalManager.cpp Connection.cpp HttpProcessor.cpp HttpHeader.cpp\
					HeaderValue.cpp HeaderProperty.cpp Utils.cpp Index.cpp PathManager.cpp

CGI_GET			:=	Multiply.cgi.cpp
CGI_POST		:=	Replace.cgi.cpp
CGI_TEST_SRC		:=	Cgi_test.cgi.cpp

OBJS			:=	$(SRCS:.cpp=.o)
INPUT			:=	"config/config.cfg"

CC				:=	g++ #c++
CC_FLAGS		:=	-Wall -Werror -Wextra -g -c -std=c++98
CLEAN_TARGETS	:=	$(wildcard $(addprefix $(OBJ_DIR), $(OBJS) $(TEST_OBJS)))
FCLEAN_TARGETS	:=	$(wildcard $(addprefix $(BIN_DIR), $(NAME) $(TEST_NAME)))\
					$(wildcard $(addprefix $(CGI_BIN_DIR), $(CGI_BIN_GET) $(CGI_BIN_POST) $(CGI_BIN_TEST)))\

COLOR_GREEN		:=	\033[0;32m
COLOR_RED		:=	\033[0;31m
COLOR_BLUE		:=	\033[0;34m
COLOR_END		:=	\033[0m

vpath %.hpp $(INC_DIR)
vpath %.cpp $(SRC_DIR) $(CGI_DIR)
vpath %.o $(OBJ_DIR)
vpath % $(BIN_DIR) $(CGI_BIN_DIR)

all: $(NAME) $(CGI_BIN_GET) $(CGI_BIN_POST) $(CGI_BIN_TEST)


$(CGI_BIN_GET):
	@$(CC) $(CGI_DIR)$(CGI_GET) -o $(CGI_BIN_DIR)$(CGI_BIN_GET)
	@echo "$(COLOR_GREEN)write file: $(CGI_BIN_DIR)$(CGI_BIN_GET)$(COLOR_END)"
	@cp $(CGI_BIN_DIR)$(CGI_BIN_GET) local42/cgi-bin/
	@echo "$(COLOR_GREEN)copy file: $(CGI_BIN_DIR)$(CGI_BIN_GET)$(COLOR_END)"

$(CGI_BIN_POST):
	@$(CC) $(CGI_DIR)$(CGI_POST) -o $(CGI_BIN_DIR)$(CGI_BIN_POST)
	@echo "$(COLOR_GREEN)write file: $(CGI_BIN_DIR)$(CGI_BIN_POST)$(COLOR_END)"
	@cp $(CGI_BIN_DIR)$(CGI_BIN_POST) local42/cgi-bin/
	@echo "$(COLOR_GREEN)copy file: $(CGI_BIN_DIR)$(CGI_BIN_POST)$(COLOR_END)"

$(CGI_BIN_TEST):
	@$(CC) $(CGI_DIR)$(CGI_TEST_SRC) -o $(CGI_BIN_DIR)$(CGI_BIN_TEST)
	@echo "$(COLOR_GREEN)write file: $(CGI_BIN_DIR)$(CGI_BIN_TEST)$(COLOR_END)"
	@cp $(CGI_BIN_DIR)$(CGI_BIN_TEST) local42/cgi-bin/
	@echo "$(COLOR_GREEN)copy file: $(CGI_BIN_DIR)$(CGI_BIN_TEST)$(COLOR_END)"


$(NAME): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(addprefix $(OBJ_DIR),$(OBJS)) -o $(BIN_DIR)$(NAME)
	@echo "$(COLOR_GREEN)write file: $(BIN_DIR)$(NAME)$(COLOR_END)"

%.o : %.cpp $(HDRS) $(MAKEFILE)
	@mkdir -p $(OBJ_DIR)
	@$(CC) -I $(INC_DIR) $(CC_FLAGS)  $< -o $(OBJ_DIR)$@
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