def build_quiz(qpool, index, altcodes):
    new_quiz = []
    question = []

    #go through qpool
    for i in index:

        #try here to catch type exception in nested loops
        try:
            question = list(qpool[i])   #get current question as list
            q_size = len(question[1])   #get answer count
            ac_size = len(altcodes)     #get altcodes count

            #remove answers, if there are more altcodes than answers
            if q_size > ac_size:
                q_size = ac_size
                question[1] = (question[1][:q_size])

            #go through the answers and edit them, if they are not already edited
            for j in range(q_size):
                if not(":" in question[1][j]):
                    question[1][j] = altcodes[j] + ": " + question[1][j]

            new_quiz.append(tuple(question))    #add edited question to our new quiz
        except IndexError:
            print("Ignoring index {} - index out of range".format(i))
        except TypeError:   #exception for type error here, to escape from nested for loops
            print("Ignoring index {} - invalid index type".format(i))
    return new_quiz

def gen_quiz(qpool, *index, altcodes=None, quiz=None):

    #no default quiz
    if not quiz:

        #set altcodes if none given
        if not altcodes:
            altcodes = "ABCDEF"
        return build_quiz(qpool, index, altcodes)   #build quiz

    #if quiz, but no altcodes
    if not altcodes:

        #no index given -> do nothing
        if not index:
            return quiz

        #otherwise, create new quiz from qpool
        new_quiz = []
        for i in index:
            try:
                new_quiz.append(tuple(qpool[i]))
            except:
                print("Ignoring index {} - index out of range".format(i))
        return new_quiz

    #if quiz and altcodes -> extend quiz
    quiz.extend(build_quiz(qpool, index, altcodes))
    return quiz
