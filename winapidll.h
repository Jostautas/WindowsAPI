#ifndef winapidll_H
#define winapidll_H
namespace dllspec{
class dllclass{
public:
    static __declspec(dllexport) bool isABomb (int ButtonID);

};
}
#endif
