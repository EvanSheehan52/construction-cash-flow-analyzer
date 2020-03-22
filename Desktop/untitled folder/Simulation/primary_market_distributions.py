import random
import numpy as np
import scipy.stats
from primary_market_securities import Security_Request, Fragment_Request, Fragment_Offer, Security, Fragment

# Template class for distributions
class Offer_Queue:

    def __init__(self, multi_round, security_request, step_size):
        self.multi_round = multi_round
        self.names = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", \
            "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"]
        self.queue = []
        self.security_request = security_request
        self.step_size = step_size


    # Uses exponential distribution over the distance from the upper bound
    def fill_queue(self, rate):

        if self.multi_round:
            num_samples = self.get_number_of_samples(rate)
        else:
            rate = 0.0
            # Give a 10% chance of failing to generate enough fragment offers
            num_samples = np.random.choice([self.security_request.num_fragments_required, \
                    int(.9 * self.security_request.num_fragments_required)], p=[.9, .1])
        # Take samples
        for i in range(num_samples):
            # Sample a lender and a rate
            lender = random.choice(self.names)
            sample_rate = self.get_sample_rate(rate)
            self.queue.append(Fragment_Offer(self.security_request, lender, sample_rate))

        self.queue.sort(key=lambda x : x.rate)

        def get_sample_rate(self):
            return None
        def get_number_of_samples(self):
            return None



# Uses exponential distribution over the distance from the upper bound
class Offer_Queue_Exponential(Offer_Queue):
    
    def __init__(self, multi_round, mean, upper_bound, security_request, step_size):
        super(Offer_Queue_Exponential, self).__init__(multi_round, security_request, step_size)
        self.mean = mean
        self.lambda_mean = 1.0 / (upper_bound - self.mean)
        self.upper_bound = upper_bound
        self.tails = 1.07 + 1 - scipy.stats.expon.cdf(self.upper_bound, scale=1 / self.lambda_mean)


    def get_number_of_samples(self, rate):
        if rate > self.upper_bound:
            return 0
        # Get the exact number of samples that should be made according to the macro distribution at this rate;
        # the macro distribution is according to distance from the upper bound
        cdf_proportion = (scipy.stats.expon.cdf(self.upper_bound - rate, scale=1 / self.lambda_mean) - \
                scipy.stats.expon.cdf(self.upper_bound - rate - self.step_size, scale=1 / self.lambda_mean)) * self.tails
        exact_number = cdf_proportion * self.security_request.num_fragments_required
        # Sample to get the number of samples from a gaussian, with a 10% standard deviation
        return int(random.normalvariate(exact_number, .1 * exact_number))


    # Sample a rate from a small gaussian centered at the current rate, else sample
    # directly from the macro distribution
    def get_sample_rate(self, rate):

        if not self.multi_round:
            sample = round(self.upper_bound - random.expovariate(self.lambda_mean), 1)
            while sample <= 0:
                sample = round(self.upper_bound - random.expovariate(self.lambda_mean), 1)
        else:
            # Sample from a normal distribution with mean at the current rate and a standard deviation
            # that is the pdf value of the rate (in the macro distribution) times 1/10 the distance between
            # the upper bound and the mean (which we use to approximate the std of an exponential distribution)
            pdf_value_std = scipy.stats.expon.pdf(self.upper_bound - rate, scale=1 / self.lambda_mean) * \
                    (self.upper_bound - self.mean) / 10
            sample = round(random.normalvariate(rate, pdf_value_std), 1)
            while sample <= 0:
                sample = round(random.normalvariate(rate, pdf_value_std), 1)
        return sample
    


# Uses normal distribution with mean, std
class Offer_Queue_Normal(Offer_Queue):
    

    def __init__(self, multi_round, mean, std, security_request, step_size):
        super(Offer_Queue_Normal, self).__init__(multi_round, security_request, step_size)
        self.mean = mean
        self.std = std
        self.tails = 1.07 + (1 - scipy.stats.norm(self.mean, self.std).cdf(self.security_request.max_rate)) + \
                scipy.stats.norm(self.mean, self.std).cdf(0.0)


    # Get the number of samples to be drawn at this rate
    def get_number_of_samples(self, rate):
        # Get the exact number of samples that should be made according to the macro distribution at this rate
        cdf_proportion = (scipy.stats.norm(self.mean, self.std).cdf(rate) - \
                scipy.stats.norm(self.mean, self.std).cdf(rate - self.step_size)) * self.tails
        exact_number = cdf_proportion * self.security_request.num_fragments_required
        # Sample to get the number of samples, with a 10% standard deviation
        return int(random.normalvariate(exact_number, .1 * exact_number))


    # Sample a rate from a small gaussian centered at the current rate if multiround, else sample
    # directly from the macro distribution
    def get_sample_rate(self, rate):

        if not self.multi_round:
            # Always fill every sample offer with a valid rate
            sample = round(random.normalvariate(self.mean, self.std), 1)
            while sample <= 0 or sample > self.security_request.max_rate:
                sample = round(random.normalvariate(self.mean, self.std), 1)
        else:
            # Sample from a normal distribution with mean at the current rate and a standard deviation
            # that is the pdf value of the rate (in the macro distribution) times the macro distribution's
            # standard deviation (so a std proportional to the pdf value of the mean's percentage of the macro std)
            pdf_value_std = scipy.stats.norm(self.mean, self.std).pdf(rate) * self.std
            sample = round(random.normalvariate(rate, pdf_value_std), 1)
            while sample <= 0:
                sample = round(random.normalvariate(rate, pdf_value_std), 1)
        return sample

