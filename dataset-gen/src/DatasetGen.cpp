#include <atomic>
#include <cassert>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <thread>

#include "BattleEngine.hpp"
#include "UnitGroups.hpp"
#include "Util.hpp"

using namespace dataset_gen;

namespace {

// Options

const char *opt_out = "dataset";

std::uint32_t opt_dataset_size = 1000;
std::uint32_t opt_smooth_size = 100;

std::uint32_t opt_max_ships = 10000;
std::uint8_t opt_max_tech = 30;

std::uint32_t opt_num_threads = 0;
std::uint32_t opt_seed = 0;

template <typename T> T parse_int_arg_or_die(const char *arg, const char *name) {
  if (arg != nullptr) {
    T result;
    if (auto [_, ec] = std::from_chars(arg, arg + std::strlen(arg), result); ec == std::errc())
      return result;
  }
  std::cerr << "Failed to parse argument " << name << '\n';
  std::exit(1);
}

void parse_args(const char *const *argv) {
  const char *arg0 = *argv++;
  for (; *argv != nullptr; ++argv) {
    if (std::strcmp(*argv, "-h") == 0 || std::strcmp(*argv, "--help") == 0) {
      std::cout << "Usage: " << arg0 << " [OPTIONS]\n"
                << '\n'
                << "Options:\n"
                << "  --dataset-size n  Dataset size (default: 1000)\n"
                << "  --max-ships n     Max number of ships in one unit group in one battle (default: 10000)\n"
                << "  --max-tech n      Max tech of a combatant (default: 30)\n"
                << "  --num-threads n   Number of threads, 0 for number of available CPUs (default: 0)\n"
                << "  --out path        Output path for the generated dataset (default: dataset)\n"
                << "  --seed n          Seed, 0 to randomly generate (default: 0)\n"
                << "  --smooth-size n   Smooth size (default: 100)\n";
      std::exit(0);
    }

    if (std::strcmp(*argv, "--dataset-size") == 0) {
      opt_dataset_size = parse_int_arg_or_die<std::uint32_t>(*++argv, "--dataset-size");
    } else if (std::strcmp(*argv, "--max-ships") == 0) {
      opt_max_ships = parse_int_arg_or_die<std::uint32_t>(*++argv, "--max-ships");
    } else if (std::strcmp(*argv, "--max-tech") == 0) {
      opt_max_tech = parse_int_arg_or_die<std::uint8_t>(*++argv, "--max-tech");
    } else if (std::strcmp(*argv, "--num-threads") == 0) {
      opt_num_threads = parse_int_arg_or_die<std::uint32_t>(*++argv, "--num-threads");
    } else if (std::strcmp(*argv, "--out") == 0) {
      opt_out = *argv;
    } else if (std::strcmp(*argv, "--seed") == 0) {
      opt_seed = parse_int_arg_or_die<std::uint32_t>(*++argv, "--seed");
    } else if (std::strcmp(*argv, "--smooth-size") == 0) {
      opt_smooth_size = parse_int_arg_or_die<std::uint32_t>(*++argv, "--smooth-size");
      if (opt_smooth_size <= 1) {
        std::cerr << "--smooth-size must be at least 2\n";
        std::exit(1);
      }
    } else {
      std::cerr << "Unknown argument " << *argv << '\n';
      std::exit(1);
    }
  }
}

void dump_settings() {
  std::cout << "Settings:\n"
            << "  dataset-path: " << opt_out << '\n'
            << "  dataset-size: " << opt_dataset_size << '\n'
            << "  smooth-size:  " << opt_smooth_size << '\n'
            << "  max-ships:    " << opt_max_ships << '\n'
            << "  max-tech:     " << static_cast<std::uint32_t>(opt_max_tech) << '\n'
            << "  num-threads:  " << opt_num_threads << '\n'
            << "  seed:         " << opt_seed << '\n';
}

Combatant gen_random_combatant(std::uint32_t random) {
  Combatant c{};

  assert(opt_max_tech < std::numeric_limits<uint8_t>::max());
  random = random_next(random);
  c.techs.weapons = static_cast<std::uint8_t>(random) % (opt_max_tech + 1);
  random = random_next(random);
  c.techs.shielding = static_cast<std::uint8_t>(random) % (opt_max_tech + 1);
  random = random_next(random);
  c.techs.armor = static_cast<std::uint8_t>(random) % (opt_max_tech + 1);

  random = random_next(random);
  std::uint32_t num_kinds = random % (Battlecruiser + 1);
  for (std::uint32_t i = 0; i < num_kinds; ++i) {
    random = random_next(random);
    std::uint8_t kind = random % (Battlecruiser + 1);
    random = random_next(random);
    c.unit_groups[kind] = random % (opt_max_ships + 1);
  }

  return c;
}

std::atomic<std::uint32_t> progress{};

struct Result {
  Combatant attacker;
  Combatant defender;
  UnitGroups<double> attacker_mean;
  UnitGroups<double> defender_mean;
  UnitGroups<double> attacker_sd;
  UnitGroups<double> defender_sd;
};

UnitGroups<double> calc_mean(const std::vector<UnitGroups<double>> &samples) {
  UnitGroups<double> sum{};
  for (const auto &sample : samples)
    sum = sum + sample;
  return sum / static_cast<double>(opt_smooth_size);
};

UnitGroups<double> calc_sd(const std::vector<UnitGroups<double>> &samples, const UnitGroups<double> &mean) {
  UnitGroups<double> sd{};
  for (const auto &sample : samples) {
    auto tmp = sample - mean;
    sd = sd + tmp * tmp;
  }
  return sq_root(sd / static_cast<double>(opt_smooth_size - 1));
}

void worker(Result *results, std::uint32_t size, std::uint32_t seed) {
  auto rng = std::mt19937{seed};
  auto random = [&] { return static_cast<std::uint32_t>(rng()); };

  std::vector<UnitGroups<double>> attacker_samples(opt_smooth_size);
  std::vector<UnitGroups<double>> defender_samples(opt_smooth_size);

  std::vector<Combatant> attackers(1);
  std::vector<Combatant> defenders(1);

  for (std::uint32_t i = 0; i < size; ++i) {
    auto attacker = gen_random_combatant(random());
    auto defender = gen_random_combatant(random());

    for (std::uint32_t j = 0; j < opt_smooth_size; ++j) {
      attackers[0] = attacker;
      defenders[0] = defender;

      fight(attackers, defenders, random());

      attacker_samples[j] = convert<double, std::uint32_t>(attackers[0].unit_groups);
      defender_samples[j] = convert<double, std::uint32_t>(defenders[0].unit_groups);
    }

    Result &res = results[i];
    res.attacker = attacker;
    res.defender = defender;
    res.attacker_mean = calc_mean(attacker_samples);
    res.defender_mean = calc_mean(defender_samples);
    res.attacker_sd = calc_sd(attacker_samples, res.attacker_mean);
    res.defender_sd = calc_sd(defender_samples, res.defender_mean);

    progress.fetch_add(1, std::memory_order_relaxed);
  }
}

} // namespace

int main(int /*argc*/, const char *const *argv) {
  std::ios::sync_with_stdio(false);

  parse_args(argv);

  if (opt_num_threads == 0)
    opt_num_threads = std::thread::hardware_concurrency();

  if (opt_seed == 0) {
    opt_seed = std::random_device{}();
    // Seed cannot be 0.
    if (opt_seed == 0)
      opt_seed = 1;
  }
  auto rng = std::mt19937{opt_seed};

  std::ofstream out_file{opt_out};
  if (!out_file.is_open()) {
    std::cerr << "Failed to open '" << opt_out << "'\n";
    return 1;
  }

  dump_settings();

  std::vector<std::thread> threads;
  threads.reserve(opt_num_threads);

  std::vector<Result> results(opt_dataset_size);
  auto res_ptr = results.data();

  for (std::uint32_t i = 0; i < opt_num_threads; ++i) {
    std::uint32_t size = opt_dataset_size / opt_num_threads;
    if (i == 0)
      size += opt_dataset_size % opt_num_threads;
    threads.push_back(std::thread{worker, res_ptr, size, rng()});
    res_ptr += size;
  }

  for (;;) {
    std::uint32_t p = progress.load(std::memory_order_relaxed);

    std::cout << '\r';
    for (std::uint32_t i = 0; i < 80; ++i)
      std::cout << ' ';
    std::cout << "\rProgress: " << p << '/' << opt_dataset_size << " " << std::setprecision(2) << std::fixed
              << 100.0 * static_cast<double>(p) / static_cast<double>(opt_dataset_size) << '%';
    std::cout.flush();

    if (p == opt_dataset_size) {
      std::cout << '\n';
      break;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds{100});
  }

  for (auto &thread : threads)
    thread.join();

  // Write the results to the dataset file.

  auto dump_techs = [&](const CombatTechs &techs) {
    out_file << static_cast<std::uint32_t>(techs.weapons) << ',' << static_cast<std::uint32_t>(techs.shielding) << ','
             << static_cast<std::uint32_t>(techs.armor);
  };
  auto dump_units = [&](const auto &units) {
    for (std::uint8_t kind = 0; kind <= Battlecruiser; ++kind) {
      out_file << static_cast<double>(units[kind]);
      if (kind != Battlecruiser)
        out_file << ',';
    }
  };
  for (const auto &result : results) {
    dump_techs(result.attacker.techs);
    out_file << ',';
    dump_techs(result.defender.techs);
    out_file << ',';
    dump_units(result.attacker.unit_groups);
    out_file << ',';
    dump_units(result.defender.unit_groups);
    out_file << ',';
    dump_units(result.attacker_mean);
    out_file << ',';
    dump_units(result.defender_mean);
    out_file << ',';
    dump_units(result.attacker_sd);
    out_file << ',';
    dump_units(result.defender_sd);
    out_file << '\n';
  }
  out_file.close();

  return 0;
}
