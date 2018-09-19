void InsertTest(int numTimes, int maxLimit, bool random)
{
  FineGrainedBST<int> list;

  using SYSTEM_CLOCK = std::chrono::system_clock;
  auto seed = SYSTEM_CLOCK::now().time_since_epoch().count();
  std::mt19937 random_engine(static_cast<unsigned>(seed));
  std::uniform_int_distribution<int> rng(0, maxLimit);

  std::ofstream file("output.txt");

  file << "Number list: ";
  if (random)
  {
    for (int i = 0; i < numTimes; ++i)
    {
      int val = rng(random_engine);
      file << val;
      list.insert(val);

      if (i != numTimes - 1)
        file << ", ";
    }
  }
  else
  {
    for (int i = 0; i < numTimes; ++i)
    {
      file << i;
      list.insert(i);

      if (i != numTimes - 1)
        file << ", ";
    }
  }

  file << std::endl;
  list.Print(file);
}

void RemoveTest(int numTimes, int numRemoved)
{
  FineGrainedBST<int> list;

  using SYSTEM_CLOCK = std::chrono::system_clock;
  auto seed = SYSTEM_CLOCK::now().time_since_epoch().count();
  std::mt19937 random_engine(static_cast<unsigned>(seed));
  std::uniform_int_distribution<int> rng(0, numTimes);

  std::ofstream file("output.txt");
  std::vector<int> numbers;
  numbers.reserve(numTimes);

  int count = 0;
  file << "Inserting numbers: " << std::endl;
  for (int i = 0; i < numTimes; ++i)
  {
    int val = rng(random_engine);
    file << val;
    bool result = list.insert(val);
    if (result)
    {
      ++count;
      numbers.push_back(val);
    }

    if (i != numTimes - 1)
      file << ", ";
  }
  file << std::endl;
  file << "Number of attempts: " << numTimes << std::endl << "Successfully inserted: " << count << std::endl;
  list.Print(file);

  std::uniform_int_distribution<int> index_dist(0, numbers.size() - 1);
  for (int i = 0; i < numRemoved; ++i)
  {
    int index = index_dist(random_engine);
    bool result = list.remove(numbers[index]);
    std::string success = (result) ? " Success" : " Failed";
    file << "Removing number: " << numbers[index] << success << std::endl;
    list.Print(file);
  }
}

void DuplicateInsertTest(int numTimes, int maxLimit)
{
  FineGrainedBST<int> list;

  using SYSTEM_CLOCK = std::chrono::system_clock;
  auto seed = SYSTEM_CLOCK::now().time_since_epoch().count();
  std::mt19937 random_engine(static_cast<unsigned>(seed));
  std::uniform_int_distribution<int> rng(0, maxLimit);
  std::ofstream file("output.txt");

  std::vector<int> vec;
  vec.reserve(numTimes);
  file << "Number list: " << std::endl;
  for (int i = 0; i < numTimes; ++i)
  {
    int val = rng(random_engine);
    file << val << " ";
    bool result = list.insert(val);

    if(result)
      vec.emplace_back(val);

    file << "Result: " << std::boolalpha << result << std::endl;
  }

  std::shuffle(vec.begin(), vec.end(), random_engine);

  file << "Inserting duplicates: " << std::endl;
  for (auto& elem : vec)
  {
    file << elem << " ";
    bool result = list.insert(elem);
    file << "Result: " << std::boolalpha << result << std::endl;
  }
  file << std::endl;
}