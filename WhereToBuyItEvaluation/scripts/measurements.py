from os import listdir
from math import sqrt
from sys import argv, float_info
import json
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
import numpy as np

confidence_level = 0.95
p = 1.96
class Measurements:

    def __init__(self, mean, standard_deviation, confidence_interval):
        self.mean = mean
        self.standard_deviation = standard_deviation
        self.confidence_interval = confidence_interval

    def toDictionary(self):
        json_dictionary = {"mean": self.mean,
                           "standard_deviation":self.standard_deviation,
                           "confidence_interval": {"lower_bound": self.confidence_interval[0],
                                                   "upper_bound": self.confidence_interval[1]}}
        return json_dictionary


def calculate_confidence_interval(base_dir):
    try:
        experiment_files = listdir(base_dir)
        results_per_file = {}
        for file in experiment_files:
            with open(base_dir+"/"+file, "r") as experiment_file:
                categories_values = []
                for line in experiment_file:
                    if line.startswith("STEP") or line.startswith("average") or line.startswith("\n"):
                        continue
                    values = line.split("\t")
                    last_value = values[-1]
                    if last_value == '\x00':
                        continue
                    categories_values.append(float(last_value))
                mean = reduce(lambda accum, x: accum+x/len(categories_values), categories_values, 0)
                variance = reduce(lambda accum,x: accum+(x-mean)**2/len(categories_values), categories_values, 0)
                standard_deviation = sqrt(variance)
                error = p*standard_deviation/sqrt(len(categories_values))
                confidence_interval = (mean-error, mean+error)
                measurements = Measurements(mean, standard_deviation, confidence_interval)
                results_per_file[file] = measurements.toDictionary()
        return results_per_file

    except OSError:
        print("Cannot open directory")
        return False


def plot_results(results_dictionary):
    lower_bound = float_info.max
    upper_bound = -1

    for (key, value) in results_dictionary.iteritems():
        interval = value["confidence_interval"]
        lower = interval["lower_bound"]
        upper = interval["upper_bound"]
        if lower_bound > lower:
            lower_bound = lower
        if upper_bound < upper:
            upper_bound = upper

    legends = []
    final_interval = np.linspace(lower_bound, upper_bound, 100)
    for (key, value) in results_dictionary.iteritems():
        interval = value["confidence_interval"]
        lower = interval["lower_bound"]
        upper = interval["upper_bound"]
        mean = value["mean"]
        standard_deviation = value["standard_deviation"]
        data = np.array(map(lambda x: np.nan if x > upper or x < lower else x, final_interval))
        normal = mlab.normpdf(data, mean, standard_deviation)
        plot = plt.plot(data, normal, label=key)
        legends.extend(plot)
    plt.legend(handles=legends)
    plt.show()


if __name__ == '__main__':
    if len(argv) < 2:
        print("missing parameters: base_dir=<dir> output=<output_file>")
        exit(1)
    base_dir_list = argv[1].split("=")
    output_file_list = argv[2].split("=")
    results = calculate_confidence_interval(base_dir_list[1])

    if results:
        json_string = json.dumps(results,sort_keys=True, indent=4, separators=(',', ': '))
        with open(output_file_list[1],"w") as output:
            output.write(json_string)
        plot_results(results)