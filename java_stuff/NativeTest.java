public class NativeTest {
    public static void main(String[] args) {
        printInt(1);
    }

    private static native void printInt(int value);
}
