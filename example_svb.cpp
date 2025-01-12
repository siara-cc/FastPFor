//
// A simple example to get you started with the library.
// You can compile and run this example like so:
//
//   make example
//   ./example
//
//  Warning: If your compiler does not fully support C++11, some of
//  this example may require changes.
//

#include <time.h>
#include <stdint.h>
#include "headers/streamvariablebyte.h"
#include "headers/deltautil.h"

double time_taken_in_secs(struct timespec t) {
  struct timespec t_end;
  clock_gettime(CLOCK_REALTIME, &t_end);
  return (t_end.tv_sec - t.tv_sec) + (t_end.tv_nsec - t.tv_nsec) / 1e9;
}

struct timespec print_time_taken(struct timespec t, const char *msg) {
  double time_taken = time_taken_in_secs(t); // in seconds
  printf("%s %lf\n", msg, time_taken);
  clock_gettime(CLOCK_REALTIME, &t);
  return t;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: example <codec> <input_file>\n  where codec is one of BP32 copy fastbinarypacking16 fastbinarypacking32 fastbinarypacking8 fastpfor128 fastpfor256 maskedvbyte newpfor optpfor pfor pfor2008 simdbinarypacking simdfastpfor128 simdfastpfor256 simdgroupsimple simdgroupsimple_ringbuf simdnewpfor simdoptpfor simdpfor simdsimplepfor simple16 simple8b simple8b_rle simple9 simple9_rle simplepfor streamvbyte varint varintg8iu varintgb vbyte vsencoding\n");
    return 1;
  }

  using namespace FastPForLib;
  // CODECFactory factory;

  // We pick a CODEC
  //IntegerCODEC &codec = *factory.getFromName("simdfastpfor256");
  //IntegerCODEC &codec = *factory.getFromName("fastpfor256");
  //IntegerCODEC &codec = *factory.getFromName("streamvbyte");
  //IntegerCODEC &codec = *factory.getFromName(argv[2]);
  //IntegerCODEC &codec = *factory.getFromName("simple8b");
  //IntegerCODEC &codec = *factory.getFromName("simdbinarypacking");
  //IntegerCODEC &codec = *factory.getFromName("varintg8iu");
  // could use others, e.g., "simdbinarypacking", "varintg8iu"
  ////////////
  //
  // create a container with some integers in it
  //
  // for this example, we will not assume that the
  // integers are in sorted order
  //
  // (Note: You don't need to use a vector.)
  //
  StreamVByte codec;
   FILE *fp = fopen(argv[1], "rb");
   char buffer[50];
   size_t line_count = 0;
   while (fgets(buffer, sizeof(buffer), fp) != NULL) {
     line_count++;
   }
   rewind(fp);
  size_t N = line_count;
  size_t line = 0;
  std::vector<uint32_t> mydata(N);
   while (fgets(buffer, sizeof(buffer), fp) != NULL) {
     mydata[line++] = atoll(buffer);
     //printf("%ld\n", mydata[line-1]);
   }
  //for (uint32_t i = 0; i < N; i += 150)
  //  mydata[i] = i;
  //
  // the vector mydata could contain anything, really
  //
  ///////////
  //
  // You need some "output" container. You are responsible
  // for allocating enough memory.
  //
  std::vector<uint32_t> compressed_output(N + 1024);
  // N+1024 should be plenty
  //
  //
  size_t compressedsize = compressed_output.size();
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  codec.encodeArray(mydata.data(), mydata.size(), compressed_output.data(),
                    compressedsize);
  printf("\nEncode kps: %lf\n", line_count / time_taken_in_secs(t) / 1000);
  t = print_time_taken(t, "Time taken for encode: ");
  //
  // if desired, shrink back the array:
  compressed_output.resize(compressedsize);
  compressed_output.shrink_to_fit();
  // display compression rate:
  std::cout << std::setprecision(3);
  std::cout << "You are using "
            << 32.0 * static_cast<double>(compressed_output.size()) /
                   static_cast<double>(mydata.size())
            << " bits per integer. " << std::endl;
  //
  // You are done!... with the compression...
  //
  ///
  // decompressing is also easy:
  //
  std::vector<uint32_t> mydataback(N);
  size_t recoveredsize = mydataback.size();
  //
  clock_gettime(CLOCK_REALTIME, &t);
  codec.decodeArray(compressed_output.data(), compressed_output.size(),
                    mydataback.data(), recoveredsize);
  printf("\nDecode kps: %lf\n", line_count / time_taken_in_secs(t) / 1000);
  t = print_time_taken(t, "Time taken for decode: ");
  printf("Compressed size: %lu\n", compressed_output.size() * 4);
  printf("Recovered size: %lu\n", recoveredsize);
  mydataback.resize(recoveredsize);
  //
  // That's it!
  //
  for (size_t i = 0; i < recoveredsize; i++) {
    if (mydataback[i] != mydata[i])
      throw std::runtime_error("bug!");
  }

  // If you need to use differential coding, you can use
  // calls like these to get the deltas and recover the original
  // data from the deltas:
  Delta::deltaSIMD(mydata.data(), mydata.size());
  Delta::inverseDeltaSIMD(mydata.data(), mydata.size());
  // be mindful of CPU caching issues

  // If you do use differential coding a lot, you might want 
  // to check out these other libraries...
  // https://github.com/lemire/FastDifferentialCoding
  // and
  // https://github.com/lemire/SIMDCompressionAndIntersection
}
