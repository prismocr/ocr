import matplotlib.pyplot as plt
import sys

def main():
    path = sys.argv[1]
	
    f = open(path, "r")
    lines = f.read().splitlines()
    epochs,batch_size, eta, lmbda = lines[0].split(' ')
    data = [line.split(' ') for line in lines[1:]]
    training_accuracy, evaluation_accuracy = [], []
    
    for d in data:
	    training_accuracy.append(float(d[0]))
	    evaluation_accuracy.append(float(d[1]))

    plt.title(f"epoch:{epochs} batch_size:{batch_size} eta:{eta} lambda:{lmbda}")

    xPos = range(1, len(data)+1)
    plt.plot(xPos, training_accuracy, label = 'training accuracy')
    plt.plot(xPos, evaluation_accuracy, label = 'evaluation accuracy')

    for var in (training_accuracy, evaluation_accuracy):
    	plt.annotate('%0.2f' % max(var), xy=(1, max(var)),
    		xytext=(8, 0), xycoords=('axes fraction', 'data'), textcoords='offset points')

    plt.xlabel('epochs')
    plt.ylabel('accuracy %')

    plt.legend()

    minY = min(min(training_accuracy), min(evaluation_accuracy))
    plt.axis([1, int(epochs), minY, 100])

    #plt.show()
    plt.savefig(path[:-3]+"png")
    f.close()

if len(sys.argv) < 2:
    raise Exception("Performance file path missing.")

if __name__ == "__main__":
    main()
