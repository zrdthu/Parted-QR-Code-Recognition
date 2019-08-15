// ------------------------
// zrd: zhangr.d.1996@gmail.com

#include "qr.h"
#include <string>
#include <vector>

using namespace std;

int main() {
    vector<string> nameList({
        "img/1.jpg",
        "img/2.jpg",
        "img/3.jpg",
        "img/4.jpg"
    });
    // iterate possible zoom factors
    vector<double> cropFactors({1.10, 1.15, 1.05, 1.00, 1.20});

    QR_Rec rec(nameList);
    cout << rec.doRec(5, cropFactors) << endl;

    return 0;
}
