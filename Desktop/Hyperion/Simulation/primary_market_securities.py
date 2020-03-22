import uuid

# Fragment size
FRAGMENT_PRINCIPAL = 100

# Request for loan/line of credit (borrower)
class Security_Request:

    def __init__(self, owner, principal, max_rate):
        # Static Information
        self.ID = uuid.uuid1().int
        self.owner = owner
        self.max_rate = max_rate

        # Rebase the principal request
        if principal % FRAGMENT_PRINCIPAL == 0:
            self.num_fragments_required = int(principal / FRAGMENT_PRINCIPAL)
            self.principal = principal
        else:
            self.num_fragments_required = int(principal / FRAGMENT_PRINCIPAL) + 1
            self.principal = self.num_fragments_required * FRAGMENT_PRINCIPAL
            print("Rebasing principal to " + str(self.principal) + ".\n")

        # Dynamic Information
        self.principal_filled = 0
        self.num_fragments_filled = 0
        self.fragments = []

    # Match a fragment request and fragment offer
    def fill_fragment(self, fragment_request, fragment_offer):
        assert(fragment_request.parent_security_ID == fragment_offer.parent_security_ID and
                fragment_request.principal == fragment_offer.principal and
                round(fragment_request.rate, 1) == round(fragment_offer.rate, 1))

        self.fragments.append((fragment_request, fragment_offer))
        self.principal_filled += fragment_request.principal
        self.num_fragments_filled += 1

# Request for fragment of security (borrower)
class Fragment_Request:

    def __init__(self, parent_security, rate, principal=FRAGMENT_PRINCIPAL):
        self.parent_security_ID = parent_security.ID
        self.owner = parent_security.owner

        self.ID = uuid.uuid1().int
        self.principal = principal
        self.rate = rate

# Offer for fragment of security (lender)
class Fragment_Offer:

    def __init__(self, parent_security, owner, rate, principal=FRAGMENT_PRINCIPAL):
        self.parent_security_ID = parent_security.ID
        self.owner = owner

        self.ID = uuid.uuid1().int
        self.principal = principal
        self.rate = rate

# Fully filled security (loan / line of credit)
class Security:

    def __init__(self, security_request):

        assert(security_request.num_fragments_required == security_request.num_fragments_filled and
                security_request.num_fragments_filled == len(security_request.fragments) and
                security_request.principal == security_request.principal_filled)

        self.ID = uuid.uuid1().int
        self.borrower = security_request.owner
        self.principal = security_request.principal
        self.fragments = [Fragment(self.ID, i[0], i[1]) for i in security_request.fragments]
        self.fragments.sort(key=lambda x : x.rate)
        self.aggregate_rate = sum([i.rate for i in self.fragments]) / float(len(self.fragments))

# Fully filled fragment of security
class Fragment:

    def __init__(self, parent_security_ID, fragment_request, fragment_offer):

        assert(fragment_request.parent_security_ID == fragment_offer.parent_security_ID and
                fragment_request.principal == fragment_offer.principal and
                round(fragment_request.rate, 1) == round(fragment_offer.rate, 1))
        
        self.parent_security_ID = parent_security_ID
        self.ID = uuid.uuid1().int
        self.borrower = fragment_request.owner
        self.lender = fragment_offer.owner

        self.principal = fragment_request.principal
        self.rate = round(fragment_request.rate, 1)


