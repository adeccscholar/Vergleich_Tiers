#pragma one

class TProcess_Reader {
public:
   TProcess_Reader() = default;
   TProcess_Reader(TProcess_Reader const&) = default;
   TProcess_Reader(TProcess_Reader&&) = default;
   virtual ~TProcess_Reader() { }

   TProcess_Reader& operator = (TProcess_Reader const&) = default;
   TProcess_Reader& operator = (TProcess_Reader&&) = default;



   };