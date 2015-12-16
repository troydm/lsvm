#ifndef AST_H
#define AST_H

#include <stdint.h>
#include <string>
#include <vector>

namespace lsvm {
    namespace ast {
        
        class Node {
            public:
                virtual ~Node() { }
        };

        class Float : public Node {
            public:
                Float(long double v) : value(v) { } 

            protected:
                long double value;
        };

        class Integer : public Node {
            public:
                Integer(int64_t v) : value(v) { } 

            protected:
                int64_t value;
        };

        class Symbol : public Node {
            public:
                Symbol(std::string n) : name(n) { } 

            protected:
                std::string name;
        };

        class Message : public Node {
            public:
                Message(Node* recv) : receiver(recv) { }

            protected:
                Node* receiver;
        };

        class UnaryMessage : public Message {
            public:
                UnaryMessage(Node* recv, Symbol* msg) : Message(recv), message(msg) { }
            protected:
                Symbol* message;
        };

        class BinaryMessage : public UnaryMessage {
            public:
                BinaryMessage(Node* recv, Symbol* msg, Node* arg) : UnaryMessage(recv,msg), argument(arg) { }
            protected:
                Node* argument;
        };

        class KeywordMessage : public Message {
            public:
                KeywordMessage(Node* recv, std::vector<Node*> kws) : Message(recv), keywords(kws) { }

            protected:
                std::vector<Node*> keywords;
        };
    
    }
}


#endif
