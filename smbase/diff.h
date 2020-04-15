#ifndef DIFF_H
#define DIFF_H


class Diff
{
public:
    Diff();
};





#include <algorithm>
#include <string>
#include "str.h"
#include <vector>

#include <stdio.h>
#include <string.h>
#include <sstream>
#include "storage.h"
#include "str.h"
#include "trace.h"

// https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Longest_common_subsequence
// See http://www-igm.univ-mlv.fr/~lecroq/seqcomp/node4.html.
class LCS
{
    template<typename T>
    class LCSTable
    {
        size_t   m_;
        size_t   n_;
        size_t*  data_;
        T *X, *Y;
        str::string both, first, second, newline;
        std::ostream * os;
        std::vector<T>* result;

    public:
        LCSTable(size_t m, size_t n, T* X, T* Y, std::vector<T>& result)
        : m_(m), n_(n), X(X), Y(Y), os(0), result(&result)
        {
            data_ = new size_t[(m_ + 1) * (n_ + 1)];
            build();
        }

        LCSTable(size_t m, size_t n, T* X, T* Y,
                 str::string &both, str::string &first, str::string &second, str::string &newline,
                 std::ostream & os)
        : m_(m), n_(n), X(X), Y(Y),
          both(both), first(first), second(second), newline(newline),
          os(&os), result(0)
        {
            data_ = new size_t[(m_ + 1) * (n_ + 1)];
            build();
        }
        ~LCSTable()
        {
            delete [] data_;
        }

        void setAt(size_t i, size_t j, size_t value)
        {
            data_[i + j * (m_ + 1)] = value;
        }

        size_t getAt(size_t i, size_t j) const
        {
            return data_[i + j * (m_ + 1)];
        }

        void backtrackOne() {
            backtrackOne(m_, n_);
        }

        bool printDiff() {
            return  printDiff(m_, n_);
        }



    private:
        void build()
        {
            for (size_t i=0; i<=m_; ++i)
                setAt(i, 0, 0);

            for (size_t j=0; j<=n_; ++j)
                setAt(0, j, 0);

            for (size_t i = 0; i < m_; ++i)
            {
                for (size_t j = 0; j < n_; ++j)
                {
                    if (X[i] == Y[j])
                        setAt(i+1, j+1, getAt(i, j)+1);
                    else
                        setAt(i+1, j+1, std::max(getAt(i+1, j), getAt(i, j+1)));
                }
            }
        }

        void backtrackOne(int i, size_t j)
        {
            result->clear();
            if (i == 0 || j == 0)
                return;
            if (X[i - 1] == Y[j - 1])
            {
                backtrackOne(i - 1, j - 1);
                result->push_back(X[i - 1]);
                return;
            }
            if (getAt(i, j - 1) > getAt(i -1, j))
                backtrackOne(i, j - 1);
            else
                backtrackOne(i - 1, j);
        }

        bool printDiff(size_t i, size_t j) {
            if (i > 0 && j > 0 && X[i-1] == Y[j-1]) {
                bool result = printDiff(i-1, j-1);
                *os << both << X[i-1] << newline;
                return result;
            } else {
                if (j > 0 && (i == 0 || getAt(i,j-1) >= getAt(i-1,j))) {
                    printDiff(i, j-1);
                    *os << second << Y[j-1] << newline;
                    return true;
                } else if (i > 0 && (j == 0 or getAt(i,j-1) < getAt(i-1,j))) {
                    printDiff(i-1, j);
                    *os << first << X[i-1] << newline;
                    return true;
                } else {
                    return false;
                }
            }
        }
    };

public:
    template<typename T>
    static void findOne(T* X, size_t m, T* Y, size_t n)
    {
        LCSTable<T> table(m, n, X, Y);
        table.backtrackOne();
    }

    template<typename T>
    static bool printDiff(std::ostream & os, std::vector<T>& X, std::vector<T>& Y,
                          str::string both="   ", str::string first="-  ",
                          str::string second="+  ", str::string newline="\n") {
        LCSTable<T> table(X.size(), Y.size(), &*X.begin(), &*Y.begin(),
                       both, first, second, newline, os);
        return table.printDiff();
    }


};




extern std::vector<str::string> rows1;
extern std::vector<str::string> rows2;

inline void splitMemoryTreeLines(str::string const &s, std::vector<str::string> &rows) {
    std::istringstream st(s.c_str());
    std::string line;
    while (std::getline(st, line, '\n')) {
        rows.reserve((rows.size()+1)<<10>>10);
        rows.push_back(line.c_str());
    }
}

#define DEBUG_MEMORY_TREE(pool, DEBUG_BLOCK)                                                \
{                                                                                           \
    try {                                                                                   \
        str::stringstream s;                                                                \
        s<<"*****************************************************************************************************\n"; \
        s<<str::lastObjName<<"..\n";                                                        \
        s<<"Pool:\n";                                                                       \
        pool.debugPrint(s);                                                                 \
        s<<"\n";                                                                            \
        s<<"String Pool:\n";                                                                \
        str_pool.debugPrint(s);                                                             \
        s<<"\n";                                                                            \
        s<<"Trace Pool:\n";                                                                 \
        trace_pool.debugPrint(s);                                                           \
        s<<"\n";                                                                            \
        DEBUG_BLOCK                                                                         \
        rows2.clear();                                                                      \
        splitMemoryTreeLines(s.str(), rows2);                                               \
        str::stringstream sd;                                                               \
        if (LCS::printDiff(sd, rows1, rows2)) {                                             \
            std::cout << sd.str();                                                          \
            std::cout<<str::flush;                                                          \
            rows1 = rows2;                                                                  \
        }                                                                                   \
        return 0;                                                                           \
    } catch (x_assert const & x) {                                                          \
        std::cout<<"assertion failed.\n";                                                   \
    }                                                                                       \
}                                                                                           \


#endif // DIFF_H
