// Copyright (c) LostPigxx. All rights reserved.
// 	      ٩(⊙o⊙*)و好!很有精神!
//
// customized comparator powered by SIMD

class Comparator {
  public:
    virtual int operator()() const = 0;
  private:
};

class StringComparator : public Comparator {
  public: 
    int operator()();
  private:
};