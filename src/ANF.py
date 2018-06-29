

import copy ;


class ANF(object):


    def __init__(self, f):
        self.f = f ;
        self.f_w = ANF.to_ANF(f) ;


    def __str__(self):
        ret = "" ;
        for i, c in enumerate(self.f_w):
            if c:
                if ret: ret+="+" ;
                ret+=ANF.to_monomial(i) ;
        return ret ;

    @staticmethod
    def to_monomial(i):
        ret = "" ;
        cpt = 0 ;
        if i==0:
            return "1" ;
        while(i > 0):
            if (i & 1):
                ret+="x_{"+str(cpt)+"}" ;
            i = i >> 1 ;
            cpt+=1
        return ret ;

    @staticmethod
    def to_ANF(f):
        f_w = copy.copy(f) ;
        f_len = len(f) ;
        step = (f_len >> 1);
        while(step > 0):
            nb_blocs = f_len//(step << 1) ;
            left = 0;
            for i in range(nb_blocs):
                right = left + step;
                for j in range(step):
                    f_w[right] = f_w[left]^f_w[right] ;
                    right+=1 ; left+=1 ;
                left = right ;
            step = (step >> 1) ;
        return f_w ;


if __name__ == "__main__":
    f = [1, 0, 1, 0, 1, 1, 1, 1] ;
    f_w = ANF(f) ;
    print(f_w) ;
