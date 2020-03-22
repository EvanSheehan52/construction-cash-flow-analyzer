import time
import random
from primary_market_securities import Security_Request, Fragment_Request, Fragment_Offer, Security, Fragment
from primary_market_distributions import Offer_Queue_Exponential, Offer_Queue_Normal

def print_security(security, verbose):

    # Print security details
    print("Security data...")
    print("ID: " + str(security.ID))
    print("Borrower: " + str(security.borrower))
    print("Principal: " + str(security.principal))
    print("Rate: " + str(security.aggregate_rate) + "\n")        

    if verbose >= 3:
        for i in range(len(security.fragments)):
            fragment = security.fragments[i]
            print("Fragment " + str(i + 1) + " data...")
            print("Security ID: " + str(fragment.parent_security_ID))
            print("ID: " + str(fragment.ID))
            print("Borrower: " + str(fragment.borrower))
            print("Lender: " + str(fragment.lender))
            print("Principal: " + str(fragment.principal))
            print("Rate: " + str(fragment.rate) + "\n")

    if verbose >= 2:
        hist = {}
        for i in security.fragments:
            if i.rate not in hist:
                hist[i.rate] = 0
            hist[i.rate] += 1
        for i in sorted(hist.keys()):
            string = ""
            for el in range(int(hist[i] / len(security.fragments) * 1000)):
                string += "#"
            if len(string) != 0:
                print(str(i) + ": " + string)

def single_round_auction(security_request, offer_queue, verbose):

    if verbose >= 2:
        print("Waiting for market...")
        print("Filling request...")

    # Fill the offer queue one time
    offer_queue.fill_queue(0.0)
    # Create fragment requests for each offer
    while len(offer_queue.queue) != 0 and round(offer_queue.queue[0].rate, 1) <= round(security_request.max_rate, 1):
        # Create a fragment request for documentation
        request = Fragment_Request(security_request, round(offer_queue.queue[0].rate, 1))
        security_request.fill_fragment(request, offer_queue.queue[0])
        offer_queue.queue.pop(0)
    
    # Check if the security request was filled or not
    if security_request.principal == security_request.principal_filled:
        # Security request fully filled!
        security = Security(security_request)
        if verbose >= 1:
            print_security(security, verbose)
    else:
        security = None
        print("Couldn\'t fill security request under max rate " + str(security_request.max_rate) + "%.")
    return security


def multi_round_auction(security_request, offer_queue, step_size, verbose):

    # Set initial rate
    rate = 0.1

    # Cycle until either the security request is filled or the offer queue has been empty for sufficiently long
    # to warrant a voided (unfillable) security request
    while security_request.principal != security_request.principal_filled and \
            rate <= security_request.max_rate:

        # Print round info
        if verbose >= 2:
            print("Waiting for market...")
            print("Filling at " + str(round(rate, 1)) + "%...")
            time.sleep(.1)

        # Additions and subtractions made to queue here
        offer_queue.fill_queue(rate) 

        # Cycle through the fragment offers at the front of the queue until the queue is empty or 
        # the first element has a bid rate greater than the ask rate
        amount_filled = 0
        while security_request.principal != security_request.principal_filled and len(offer_queue.queue) != 0:

            if round(offer_queue.queue[0].rate, 1) <= round(rate, 1):
                # Transaction made!
                # Lower the market asking rate
                #if round(offer_queue.queue[0].rate, 1) < round(rate, 1):
                    #rate = offer_queue.queue[0].rate

                # Create a fragment request for documentation
                request = Fragment_Request(security_request, round(offer_queue.queue[0].rate, 1))
                security_request.fill_fragment(request, offer_queue.queue[0])
                amount_filled += offer_queue.queue[0].principal
                offer_queue.queue.pop(0)
            # If lowest bid exceeds the current ask, end the round
            else:
                break

        # Print round info
        if verbose >= 2:
            print("Filled " + str(amount_filled) + " of " + str(security_request.principal) + " this round.")
            print("Cumulatively filled " + str(security_request.principal_filled) + " of " + str(security_request.principal))
            print("Raising rate to " + str(round(rate + step_size, 1)) + "...\n")

        # Raise rate
        rate = rate + step_size

    # Check if the security request was filled or not
    if security_request.principal == security_request.principal_filled:
        # Security request fully filled!
        security = Security(security_request)
        if verbose >= 1:
            print_security(security, verbose)
    else:
        security = None
        print("Couldn\'t fill security request under max rate " + str(security_request.max_rate) + "%.")
    return security

#############################################################################
#                         Temporary driver stuff                            #
#############################################################################

# Params:   borrower - String for borrower's name
#           principal - Integer for princiapl amount (divisable by 100 preferably) 
#           max_rate - Float for maximum rate acceptable to borrower
#           auction - String "single" for single round auction, "multi" for multi round auction
#
#           offer simulator: dist - "exp" for exponential distribution, "normal" for normal distribution,
#           if "exp" - 
#               param1 - Float for mean of exponential distribution
#               param2 - Float for upper bound
#               Best results are received when mean is ~2 lower than the upper bound
#           if "normal" - 
#               param1 - Float for mean of normal distribution
#               param2 - Float for standard deviation
#           
#           step_size - Float for the rate increment in the multi-round auction format
#           verbose - Int corresponding to level of verbosity
def driver(borrower, principal, max_rate, auction, dist, param1, param2, step_size=.1, verbose=2):

    assert((auction == "single" or auction == "multi") and (dist == "exp" or dist == "normal"))

    # Create the security request from the given parameters
    security_request = Security_Request(borrower, principal, max_rate)
    # Set the auction format
    if auction == "single":
        multi = False
    elif auction == "multi":
        multi = True

    # Set the distribution format
    if dist == "exp":
        offer_queue = Offer_Queue_Exponential(multi, param1, param2, security_request, step_size)
    elif dist == "normal":
        offer_queue = Offer_Queue_Normal(multi, param1, param2, security_request, step_size)

    # Run the auction
    if not multi:
        return single_round_auction(security_request, offer_queue, verbose)
    else:
        return multi_round_auction(security_request, offer_queue, step_size, verbose)

